#include <iostream>
#include <vector>
#include <random>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>

#include "SDLWindow.h"
#include "ShaderUtils.h"
#include "Mesh.h"
#include "Camera.h"
#include "Pass.h"
#include "Graph.h"
#include "FrameBufferObject.h"


SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

bool mouseDown = false;
float glX;
float glY;
Model* cubeModel;

std::vector<Model> modelList;

Camera fpsCamera(glm::vec3(0.0f,0.0f,3.0f));

GLuint renderShader; 

Graph defferedShadingGraph;


FrameBufferObject gBufferFBO;
// TextureObj GPos, GNorm, GAlbSpec;

TextureObj wallTex;

TextureFormat GPosFmt = {GL_RGB16F, GL_RGBA, GL_FLOAT };
TextureFormat GNormFmt = {GL_RGB16F, GL_RGBA, GL_FLOAT };
TextureFormat GAbSpFmt = {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };

std::vector<std::string> outputs = {"gPosition","gNormal","gAlbedoSpec"};
std::vector<TextureFormat> outputFmts = {GPosFmt, GNormFmt, GAbSpFmt};

// Pass lightspass("shaders/LightVertex.glsl","shaders/LightFragment.glsl",fpsCamera,{}, true, outputs,{},outputFmts,{});
// Pass GbufferPass("shaders/DSVertex.glsl", "shaders/DSFragment.glsl",fpsCamera,modelList,false,{},outputs,{},outputFmts);

const unsigned int NR_Lights = 32;
std::vector<glm::vec3> lightPos;
std::vector<glm::vec3> lightCol;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

unsigned int texture;




void GetOpenGLVersionInfo() 
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void InitialiseSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL Video Subsystem: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_Init");

    std::cout << std::filesystem::current_path() << std::endl;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    GraphicsApplicationWindow = SDL_CreateWindow("GLWindow", 0, 0, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL);
    if (GraphicsApplicationWindow == nullptr) {
        std::cerr << "SDL_Window was not created: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_CreateWindow");

    OpenGlConext = SDL_GL_CreateContext(GraphicsApplicationWindow);
    if (OpenGlConext == nullptr) {
        std::cerr << "OpenGL Context was not created: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_GL_CreateContext");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    CheckGLError("glewInit");

    GetOpenGLVersionInfo();
    std::cout << "OpenGL initialized successfully!" << std::endl;
}

void InitialiseProgram() 
{
    InitialiseSDL();

    glEnable(GL_DEPTH_TEST);


    std::string modelPath = "models/test2.obj";

    /*
    !!!
    TO DO 
    Add a scene graph for geometry and lights
    !!! 
    */


    Model cubeModel(modelPath);
    Model cubeModel2(modelPath);

    cubeModel2.translate(glm::vec3(2.0f,0.0f,0.0f));

    modelList.push_back(cubeModel);
    modelList.push_back(cubeModel2);

    renderShader = loadShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    // GbufferPass.init();
    // lightspass.init();
    CheckGLError("Pass init");


    wallTex.create("walltex", ScreenWidth,ScreenHeight,GPosFmt,GL_COLOR_ATTACHMENT0,"textures/white-brick-wall-seamless-texture-free.png",true);
    CheckGLError("Texture Loading");

    // GbufferPass.drawBuffers();
    // GbufferPass.depthBufferSetup();
    CheckGLError("GBuffer Creation");

    defferedShadingGraph.readJson("example.json");

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Gbuffer Complete" << std::endl;
    }

    // lights


    srand(13);
    for (unsigned int i = 0; i < NR_Lights; i++)
    {
        std::random_device rd;
        std::uniform_real_distribution<double> posDist(-0.7f,0.7f);
        std::uniform_real_distribution<double> ColDist(0.1,1.0);
        float xPos = posDist(rd);
        float yPos = posDist(rd);
        float zPos = posDist(rd);
        lightPos.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = ColDist(rd);
        float gColor = ColDist(rd);
        float bColor = ColDist(rd);
        lightCol.push_back(glm::vec3(rColor, gColor, bColor));
    }

}

void Input() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) 
    {

        // Quit event
        if (e.type ==  SDL_QUIT)
        {
            std::cout << "Bye!" << std::endl;
            gQuit = true;
        }

        
        if (e.type == SDL_MOUSEBUTTONDOWN) 
        {

            if (e.button.button == SDL_BUTTON_LEFT) 
            {
                mouseDown = true;
                glX = ( e.motion.x); 
                glY = ((ScreenHeight - e.motion.y));
            }
        }
        if (e.type == SDL_MOUSEMOTION) 
        {
            if (e.motion.state & SDL_BUTTON_LMASK) 
            {
                float xpos =  static_cast<float>(e.motion.x);
                float ypos =  static_cast<float>(ScreenHeight- e.motion.y);

                float xoffset = xpos - glX;
                float yoffset = ypos - glY;
                glX = xpos;
                glY = ypos;
                fpsCamera.processMouseMovement(xoffset,yoffset);
            }
            
        }
        if (e.type == SDL_MOUSEBUTTONUP) 
        {
            if (e.button.button == SDL_BUTTON_LEFT) 
            {
                mouseDown = false;
                glX = 0.0; 
                glY = 0.0;  
                
            }
        }
    }

    // WASD Movement 

    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_W]) fpsCamera.Move(FORWARD, deltaTime);
    if (keystates[SDL_SCANCODE_S]) fpsCamera.Move(BACKWARD, deltaTime);
    if (keystates[SDL_SCANCODE_A]) fpsCamera.Move(LEFT, deltaTime);
    if (keystates[SDL_SCANCODE_D]) fpsCamera.Move(RIGHT, deltaTime);
    if (keystates[SDL_SCANCODE_E]) fpsCamera.Move(UP, deltaTime);
    if (keystates[SDL_SCANCODE_Q]) fpsCamera.Move(DOWN, deltaTime);

}


void MainLoop() {
    while (!gQuit) 
    {
        Input();

        // Get Delta time

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000) / SDL_GetPerformanceFrequency();
        deltaTime /= 1000.0;

        defferedShadingGraph.clearBuffers();
        CheckGLError("Clear Color");

        // GBuffer Pass

        // GbufferPass.frameBuffer.bind();
        // GbufferPass.clear();
        // GbufferPass.execute();
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, wallTex.texID);

        // GbufferPass.loadViewProjMatricies(fpsCamera);
        // for (Model m : modelList)
        // {
        //     GbufferPass.loadModelMatricies(m.transMat);
        //     m.Draw();

        // }

        

        // glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // CheckGLError("GBuffer Pass");


        // // Light Pass

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glUseProgram(lightspass.shaderProgram);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[0].texID);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[1].texID);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[2].texID);
        // CheckGLError("Light Pass");
        

        // for (unsigned int i = 0; i < lightPos.size(); i++)
        // {
        //     setVec3(lightspass.shaderProgram,("lights[" + std::to_string(i) + "].Position"), lightPos[i]);
        //     setVec3(lightspass.shaderProgram,("lights[" + std::to_string(i) + "].Color"), lightCol[i]);
        //     const float linear = 0.7f;
        //     const float quadratic = 1.8f;
        //     setFloat(lightspass.shaderProgram,("lights[" + std::to_string(i) + "].Linear"), linear);
        //     setFloat(lightspass.shaderProgram,("lights[" + std::to_string(i) + "].Quadratic"), quadratic);
        // }
        // setVec3(lightspass.shaderProgram,"viewPos", fpsCamera.CamPos);
        // CheckGLError("Light creation");

        // glDisable(GL_DEPTH_TEST);

        // // Render screen quad

        // renderQuad();

        // glBindFramebuffer(GL_READ_FRAMEBUFFER, GbufferPass.frameBuffer.getID());
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // glBlitFramebuffer(
        // 0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        // );
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);

        // render cubes for lights

        glUseProgram(renderShader);

        glm::mat4 view = fpsCamera.getView();
        glm::mat4 projection = glm::perspective(glm::radians(fpsCamera.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);
        setMat4(renderShader, "projection", projection);
        setMat4(renderShader, "view", view);

        for (unsigned int i = 0; i < lightPos.size(); i ++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos[i]);
            model = glm::scale(model, glm::vec3(0.05f));
            setMat4(renderShader, "model", model);
            setVec3(renderShader,"lightColor", lightCol[i]);
            glUseProgram(renderShader);
            renderCube();
        }

        SDL_GL_SwapWindow(GraphicsApplicationWindow);
        


    }
}

void CleanUp() 
{
    glDeleteProgram(renderShader);
    SDL_GL_DeleteContext(OpenGlConext);
    SDL_DestroyWindow(GraphicsApplicationWindow);
    SDL_Quit();
}

int main() 
{
    InitialiseProgram();
    MainLoop();
    CleanUp();
    return 0;
}