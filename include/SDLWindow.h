#ifndef SDLWINDOW_H
#define SDLWINDOW_H
#include <SDL2/SDL.h>

void GetOpenGLVersionInfo();
void InitialiseProgram();
void Input();
void RenderQuad();
void MainLoop();
void CleanUp();


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


#endif