#include "FrameBufferObject.h"

#include "stb_image.h"
#include <iostream>


void TextureObj::create(std::string inName, int w, int h, TextureFormat& fmt, GLenum attachment,bool isCompute, const std::string& inTexFile, bool isImageTex)
{
    name = inName;
    width = w;
    height = h;
    format = fmt;
    attachmentPoint = attachment;
    filePath = inTexFile;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    

    if (isImageTex)
    {
        int width, height, channels;
        std::string fullpath = (ASSET_DIR + filePath);
        const char* stbi_path = fullpath.c_str();
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
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, fmt.internalFormat, w, h, 0, fmt.format, fmt.type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }



}


void FrameBufferObject::create()
{
    glGenFramebuffers(1,&fbo);
}

void FrameBufferObject::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
}

void FrameBufferObject::attachTexture(TextureObj tex)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, tex.attachmentPoint, GL_TEXTURE_2D, tex.texID, 0);
    // Textures.push_back(tex);
}

GLuint FrameBufferObject::getID() 
{
    return fbo;
}