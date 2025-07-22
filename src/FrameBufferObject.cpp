#include "FrameBufferObject.h"

void TextureObj::create(std::string inName, int w, int h, TextureFormat& fmt, GLenum attachment, bool isImageTex)
{
    name = inName;
    width = w;
    height = h;
    format = fmt;
    attachmentPoint = attachment;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt.internalFormat, w, h, 0, fmt.format, fmt.type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, tex.attachmentPoint, GL_TEXTURE_2D, tex.texID, 0);
    // Textures.push_back(tex);
}

GLuint FrameBufferObject::getID() 
{
    return fbo;
}