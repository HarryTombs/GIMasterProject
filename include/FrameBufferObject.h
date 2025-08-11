#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H 
#include <GL/glew.h>
#include <vector>
#include <string>


struct TextureFormat
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

struct TextureObj
{
public: 
    TextureObj(bool isImageTex = false){
        useImageTex = isImageTex;
    }
    std::string name;
    int width;
    int height;
    TextureFormat format;
    GLenum attachmentPoint;
    GLuint texID;
    bool useImageTex;
    std::string filePath;
    
    void create(std::string inName, int w, int h, TextureFormat& fmt, GLenum attachment, bool isCompute = false, const std::string& inTexFile = "",bool isImageTex = false);

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