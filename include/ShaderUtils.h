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

GLuint loadShaderProgram(const std::string& vertPath, const std::string& fragPath) {
    auto load = [](const std::string& path, GLenum type) {
        GLuint shader = glCreateShader(type);
        std::string src = readFile(path);
        const char* cstr = src.c_str();
        glShaderSource(shader, 1, &cstr, NULL);
        glCompileShader(shader);
        return shader;
    };
    GLuint vert = load(vertPath, GL_VERTEX_SHADER);
    GLuint frag = load(fragPath, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    return program;
}

void setMat4(GLuint program, const std::string& name, const glm::mat4 &value) 
{
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);

}