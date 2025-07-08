#pragma once
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>


std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint loadComputeShader(const std::string& path) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    std::string src = readFile(path);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);
    if (glGetError() != GL_NO_ERROR) {
        std::cerr << "Error compiling compute shader: " << path << std::endl;
        return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Compute Shader Compilation Failed:\n" << infoLog << std::endl;
    }
    return program;
}

GLuint loadShaderProgram(const std::string& inVertPath, const std::string& inFragPath) {
    auto load = [](const std::string& path, GLenum type) {
        GLuint shader = glCreateShader(type);
        std::string src = readFile(path);
        const char* cstr = src.c_str();
        glShaderSource(shader, 1, &cstr, NULL);
        glCompileShader(shader);
        return shader;
    };
    std::string vertPath = (std::string(ASSET_DIR) + inVertPath);
    std::string fragPath = (std::string(ASSET_DIR) + inFragPath);

    GLuint vert = load(vertPath, GL_VERTEX_SHADER);
    GLuint frag = load(fragPath, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        return 0;
    }
    std::cout << "Shader Program Linked Successfully: " << inVertPath << " and " << inFragPath << std::endl;
    return program;
}

void setFloat(GLuint program, const std::string& name, float value)
{
    GLuint location = glGetUniformLocation(program, name.c_str());
    glUniform1f(location, value);
}

void setMat4(GLuint program, const std::string& name, const glm::mat4 &value) 
{
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void setVec3(GLuint program, const std::string& name, const glm::vec3 &value)
{
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform3fv(location,1,&value[0]);
}