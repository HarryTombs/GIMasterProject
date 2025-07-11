#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H 
#include <GL/glew.h>
#include <vector>

struct TextureFormat
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

struct TextureObj
{
public: 
    int width;
    int height;
    TextureFormat format;
    GLenum attachmentPoint;
    void *data;
    GLuint texID;
    
    void createTex(int w, int h, TextureFormat& fmt, GLenum attachment);

};

class FrameBufferObject 
{
public: 
    GLuint fbo;
    std::vector<TextureObj> Textures;

    void create();
    void bind();
    void attachTexture(TextureObj tex);
    GLuint getID();
    
private:
};

#endif