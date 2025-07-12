#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>

#include "SDLWindow.h"
#include "ShaderUtils.h"
#include "Mesh.h"
#include "Camera.h"
#include "FrameBufferObject.h"

int ScreenHeight = 512;
int ScreenWidth = 512;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

bool upPress = false;
bool downPress = false;
bool rightPress = false;
bool leftPress = false;

bool mouseDown = false;
float glX;
float glY;
Model* cubeModel;
Camera fpsCamera(glm::vec3(0.0f,0.0f,3.0f));

GLuint renderShader, gbufferShader, lightShader; 
GLuint quadVAO,quadVBO;

FrameBufferObject gBufferFBO;
TextureObj GPos, GNorm, GAlbSpec;

TextureFormat GPosFmt = {GL_RGB16F, GL_RGBA, GL_FLOAT };
TextureFormat GNormFmt = {GL_RGB16F, GL_RGBA, GL_FLOAT };
TextureFormat GAbSpFmt = {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };

const unsigned int NR_Lights = 32;
std::vector<glm::vec3> lightPos;
std::vector<glm::vec3> lightCol;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

unsigned int texture;


float screenQuadVerticies[] = 
{
    1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
};


void CheckSDLError(const std::string& message) 
{
    const char* error = SDL_GetError();
    if (*error != '\0') 
    {
        std::cerr << "SDL Error (" << message << "): " << error << std::endl;
        SDL_ClearError();
        exit(1);
    }
}

void CheckGLError(const std::string& message) 
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) 
    {
        std::cerr << "OpenGL Error (" << message << "): " << error << std::endl;
        exit(1);
    }
}

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

    cubeModel = new Model(modelPath);

    renderShader = loadShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    gbufferShader = loadShaderProgram("shaders/DSVertex.glsl", "shaders/DSFragment.glsl");
    lightShader = loadShaderProgram("shaders/LightVertex.glsl", "shaders/LightFragment.glsl");

    if (renderShader == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
        exit(1);
    }

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, channels;
    constexpr const char* stbi_path = (ASSET_DIR "textures/white-brick-wall-seamless-texture-free.png");
    unsigned char *data = stbi_load(stbi_path, &width, &height, &channels, 4);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        std::cerr << "Failed to load texture: " << stbi_path << std::endl;
        std::cerr << "stbi error: " << stbi_failure_reason() << std::endl;

    }
    stbi_image_free(data);
    CheckGLError("Texture Loading");

    // Create Screen Quad

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(screenQuadVerticies),&screenQuadVerticies,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)(3*sizeof(float)));

    /// GBuffer Creation

    gBufferFBO.create();
    gBufferFBO.bind();

    // Texture Creation
    
    GPos.create(ScreenWidth,ScreenHeight,GPosFmt,GL_COLOR_ATTACHMENT0);
    gBufferFBO.attachTexture(GPos);

    GNorm.create(ScreenWidth,ScreenHeight,GNormFmt,GL_COLOR_ATTACHMENT1);
    gBufferFBO.attachTexture(GNorm);

    GAlbSpec.create(ScreenWidth,ScreenHeight,GAbSpFmt,GL_COLOR_ATTACHMENT2);
    gBufferFBO.attachTexture(GAlbSpec);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScreenWidth , ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGLError("GBuffer Creation");

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Gbuffer Complete" << std::endl;
    }

    glUseProgram(lightShader);
    glUniform1i(glGetUniformLocation(lightShader, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(lightShader, "gNorm"), 1);
    glUniform1i(glGetUniformLocation(lightShader, "gAlbedoSpec"), 2);

    // lights


    srand(13);
    for (unsigned int i = 0; i < NR_Lights; i++)
    {
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 0.5 - 0.2);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 1.0 - 0.2);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 1.0 - 0.1);
        lightPos.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        lightCol.push_back(glm::vec3(rColor, gColor, bColor));
    }

    // !!!!
    // change this i don't like this setup
    // !!!!
}

void LoadMatricies (GLuint shaderProgram) 
{
    glUseProgram(shaderProgram);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = fpsCamera.getView();
    glm::mat4 projection = glm::perspective(glm::radians(fpsCamera.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    float angle = static_cast<float>(SDL_GetTicks()) / 1000.0f * 50.0f; 
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.5f, 0.1f));

    setMat4(shaderProgram, "model", model);  
    setMat4(shaderProgram, "view", view);
    setMat4(shaderProgram, "projection", projection);
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

        // glClearColor(0.8, 0.4, 0.15, 1);
        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CheckGLError("Clear Color");

        // GBuffer Pass

        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO.getID());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        LoadMatricies(gbufferShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        cubeModel->Draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CheckGLError("GBuffer Pass");

        // Light Pass

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(lightShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GPos.texID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, GNorm.texID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, GAlbSpec.texID);
        CheckGLError("Light Pass");
        

        for (unsigned int i = 0; i < lightPos.size(); i++)
        {
            setVec3(lightShader,("lights[" + std::to_string(i) + "].Position"), lightPos[i]);
            setVec3(lightShader,("lights[" + std::to_string(i) + "].Color"), lightCol[i]);

            const float linear = 0.7f;
            const float quadratic = 1.8f;
            setFloat(lightShader,("lights[" + std::to_string(i) + "].Linear"), linear);
            setFloat(lightShader,("lights[" + std::to_string(i) + "].Quadratic"), quadratic);
        }
        setVec3(lightShader,"viewPos", fpsCamera.CamPos);
        CheckGLError("Light creation");

        // Render screen quad

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        // glBindTexture(GL_TEXTURE_2D, texture);

        // CheckGLError("Bind Texture");

        // LoadMatricies(renderShader);

        // cubeModel->Draw();

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