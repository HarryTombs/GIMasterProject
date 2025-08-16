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
#include "Scene.h"
#include "Pass.h"
#include "Graph.h"
#include "FrameBufferObject.h"


SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

bool mouseDown = false;
float glX;
float glY;




GLuint renderShader, computeShader; 

unsigned int sdfBuffer;
unsigned int probeBuffer;
unsigned int lightBufffer;
unsigned int indirectBuffer;

bool rebakeLighting = false;

Scene scene;

Graph defferedShadingGraph;

TextureObj inTexture;
TextureFormat fmt = {GL_RGBA,GL_RGBA,GL_FLOAT};

TextureObj computeTexture;
TextureFormat comFmt = {};

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

    Uint32 windowflags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    GraphicsApplicationWindow = SDL_CreateWindow("GLWindow", 0, 0, ScreenWidth, ScreenHeight, windowflags);
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

    scene.init();

    computeShader = loadComputeShader("shaders/compute.glsl");
    renderShader = loadShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    CheckGLError("Shaders");

    defferedShadingGraph.currentCam = &scene.currentCam;
    defferedShadingGraph.initGraph("example.json",scene);
    CheckGLError("JsonLoad");

    glGenBuffers(1,&sdfBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sdfBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(SDFPrim)*scene.sdfprims.size(), scene.sdfprims.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0, sdfBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1,&probeBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, probeBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(Probe)*scene.probes.size(), scene.probes.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1, probeBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1,&lightBufffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBufffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(SpotLight)*scene.Lights.size(), scene.Lights.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2, lightBufffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1,&indirectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * scene.probes.size(), nullptr,GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indirectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    CheckGLError("ssbo Buffer setup");

    struct Vec3Padded
    {
        glm::vec3 color;
        float pad;
    };
    std::vector<Vec3Padded> debugColors(scene.probes.size());
    for (int i = 0; i < scene.probes.size(); i++)
    {
        // Simple color gradient based on probe index
        float t = float(i) / float(scene.probes.size() - 1);
        debugColors[i].color = glm::vec3(1.0f - t, t, 0.5f);
        debugColors[i].pad = 0.0f;
    }

    // Upload debug data to GPU

    glGenBuffers(1,&indirectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vec3Padded) * scene.probes.size(), debugColors.data(),GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indirectBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    CheckGLError("ssbo Buffer setup");



    rebakeLighting = true;


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

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            ScreenWidth = e.window.data1;
            ScreenHeight = e.window.data2;
            std::cout << "w " << ScreenWidth << " h " << ScreenHeight << std::endl;
            
            defferedShadingGraph.resizeTextures();
            glViewport(0,0,ScreenWidth,ScreenHeight);
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
                scene.currentCam.processMouseMovement(xoffset,yoffset);
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
    if (keystates[SDL_SCANCODE_W]) scene.currentCam.Move(FORWARD, deltaTime);
    if (keystates[SDL_SCANCODE_S]) scene.currentCam.Move(BACKWARD, deltaTime);
    if (keystates[SDL_SCANCODE_A]) scene.currentCam.Move(LEFT, deltaTime);
    if (keystates[SDL_SCANCODE_D]) scene.currentCam.Move(RIGHT, deltaTime);
    if (keystates[SDL_SCANCODE_E]) scene.currentCam.Move(UP, deltaTime);
    if (keystates[SDL_SCANCODE_Q]) scene.currentCam.Move(DOWN, deltaTime);

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

        // if (rebakeLighting)
        // {
        //     glUseProgram(computeShader);

        //     setInt(computeShader,"numProbes", scene.probes.size());
        //     setInt(computeShader,"numSDFs", scene.sdfprims.size());
        //     setInt(computeShader,"numLights", scene.Lights.size());
        //     glUniform2i(glGetUniformLocation(computeShader, "Resolution"), ScreenWidth, ScreenHeight);

        //     glDispatchCompute(scene.probes.size(), 1, 1);
        //     std::cout <<scene.probes.size() << std::endl;
        //     glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //     rebakeLighting = false;
        //     std::cout << "Rebaked Lighting " << std::endl;
        //     CheckGLError("Compute Shader Dispatch");
        // }
        

        

        defferedShadingGraph.mainLoop();

        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, defferedShadingGraph.passes[0]->frameBuffer.getID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
        glBlitFramebuffer(
        0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        glEnable(GL_DEPTH_TEST);

        // render cubes for lights
        CheckGLError("run graph");
        

        glUseProgram(renderShader);

        glm::mat4 view = scene.currentCam.getView();
        glm::mat4 projection = glm::perspective(glm::radians(scene.currentCam.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);
        setMat4(renderShader, "projection", projection);
        setMat4(renderShader, "view", view);

        for (unsigned int i = 0; i < scene.Lights.size(); i ++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, scene.lightPos[i]);
            model = glm::scale(model, glm::vec3(0.05f));
            setMat4(renderShader, "model", model);
            setVec3(renderShader,"lightColor", scene.lightCol[i]);
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