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

std::vector<Model> modelList;

Camera fpsCamera(glm::vec3(0.0f,0.0f,3.0f));

GLuint renderShader; 

Graph defferedShadingGraph;

TextureObj inTexture;
TextureFormat fmt = {GL_RGBA,GL_RGBA,GL_FLOAT};

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
    CheckGLError("Shaders");

    inTexture.create("inTex",ScreenWidth,ScreenHeight,fmt,GL_COLOR_ATTACHMENT0,"textures/7051776139_0a12399c9c_o.png",true);

    defferedShadingGraph.initGraph("example.json");

    CheckGLError("JsonLoad");


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

        defferedShadingGraph.passes[0]->frameBuffer.bind();
        defferedShadingGraph.passes[0]->clear();

        

        defferedShadingGraph.passes[0]->loadViewProjMatricies(fpsCamera);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,inTexture.texID);

        for (Model m : modelList)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m.pos);
            setMat4(defferedShadingGraph.passes[0]->shaderProgram, "model", model);
            m.Draw();
        }


        
        // defferedShadingGraph.executePasses();

        
        glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CheckGLError("GBuffer Pass");


        // // Light Pass

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(defferedShadingGraph.passes[1]->shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defferedShadingGraph.textures["GPos"].texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, defferedShadingGraph.textures["GNorm"].texID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, defferedShadingGraph.textures["GAlbeSpec"].texID);

        // glUseProgram(lightspass.shaderProgram);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[0].texID);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[1].texID);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, GbufferPass.newOutsobjs[2].texID);
        CheckGLError("Light Pass");
        

        for (unsigned int i = 0; i < lightPos.size(); i++)
        {
            setVec3(defferedShadingGraph.passes[1]->shaderProgram,("lights[" + std::to_string(i) + "].Position"), lightPos[i]);
            setVec3(defferedShadingGraph.passes[1]->shaderProgram,("lights[" + std::to_string(i) + "].Color"), lightCol[i]);
            const float linear = 0.7f;
            const float quadratic = 1.8f;
            setFloat(defferedShadingGraph.passes[1]->shaderProgram,("lights[" + std::to_string(i) + "].Linear"), linear);
            setFloat(defferedShadingGraph.passes[1]->shaderProgram,("lights[" + std::to_string(i) + "].Quadratic"), quadratic);
        }
        setVec3(defferedShadingGraph.passes[1]->shaderProgram,"viewPos", fpsCamera.CamPos);
        CheckGLError("Light creation");

        glDisable(GL_DEPTH_TEST);

        // // Render screen quad

        renderQuad();

        // glBindFramebuffer(GL_READ_FRAMEBUFFER, GbufferPass.frameBuffer.getID());
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // glBlitFramebuffer(
        // 0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        // );
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);

        // render cubes for lights
        CheckGLError("run graph");
        

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