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

// CITED FROM learnopengl.com 

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCube()
{
    if (cubeVAO == 0)
    {
        float cubeVerticies[] = 
            {
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies), &cubeVerticies, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            std::cout << "CubeMade" << std::endl;
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    CheckGLError("CubeMaking");
    
}

// end of citation

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

    /// THESE ARE JUST INTS YOU CAN DO GL_COLOR_ATTACH + 1

    GNorm.create(ScreenWidth,ScreenHeight,GNormFmt,GL_COLOR_ATTACHMENT1);
    gBufferFBO.attachTexture(GNorm);

    GAlbSpec.create(ScreenWidth,ScreenHeight,GAbSpFmt,GL_COLOR_ATTACHMENT1 + 1);
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

    // !!!!
    // Fix the lights in the shader somethings wrong its only picking one
    // !!!!
}

void LoadMatricies (GLuint shaderProgram, bool useModelMatrix) 
{
    glUseProgram(shaderProgram);

    glm::mat4 view = fpsCamera.getView();
    glm::mat4 projection = glm::perspective(glm::radians(fpsCamera.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);

    if (useModelMatrix == true)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        float angle = static_cast<float>(SDL_GetTicks()) / 1000.0f * 50.0f; 
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.5f, 0.1f));
        setMat4(shaderProgram, "model", model);  
    }    

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

        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CheckGLError("Clear Color");

        // GBuffer Pass

        gBufferFBO.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        LoadMatricies(gbufferShader,true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        cubeModel->Draw();
        glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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

        glDisable(GL_DEPTH_TEST);

        // Render screen quad

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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