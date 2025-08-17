#pragma once
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>


extern int ScreenHeight;
extern int ScreenWidth;

extern unsigned int cubeVAO;
extern unsigned int cubeVBO;

extern unsigned int quadVAO;
extern unsigned int quadVBO;

extern unsigned int frameCount;

void CheckSDLError(const std::string& message);
void CheckGLError(const std::string& message) ;

void renderQuad();
void renderCube();

std::string readFile(const std::string& path);
GLuint loadComputeShader(const std::string& path);
GLuint loadShaderProgram(const std::string& inVertPath, const std::string& inFragPath);

void setInt (GLuint program, const std::string& name, int value);
void setFloat(GLuint program, const std::string& name, float value);
void setMat4(GLuint program, const std::string& name, const glm::mat4 &value);
void setVec3(GLuint program, const std::string& name, const glm::vec3 &value);
void setVec2(GLuint program, const std::string& name, const glm::vec2 &value);