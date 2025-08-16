#ifndef TEXTUREUTILS_H
#define TEXTUREUTILS_H

#include <GL/glew.h>
#include <string>
#include <iostream>

GLenum getGLEnumFromString(const std::string& str);

struct TextureConfig
{
    std::string name;
    bool existing;
    int width;
    int height;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLenum attachmentPoint;
    bool isImageTex;
    std::string imageFile;
};


#endif