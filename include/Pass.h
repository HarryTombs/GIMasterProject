#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include "SDLWindow.h"
#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <string>
#include <rapidjson/document.h>

using namespace rapidjson;

// forward declaration
class Graph;

class Pass
{
public:

    Pass(std::string vertpath, std::string fragpath, Camera cam, std::vector<Model> models = {}, bool screenQuad = false,
        const std::vector<std::string>& texturesIn = {}, const std::vector<std::string>& texturesOut = {},
        const std::vector<TextureFormat> formatIn = {},const std::vector<TextureFormat> formatOut = {});

    Graph* graph = nullptr;

    std::string name;

    std::vector<std::string> In;
    std::vector<std::string> Out;
    std::vector<TextureFormat> InFmt;
    std::vector<TextureFormat> OutFmt;

    std::vector<GLenum> attachments;

    std::vector<TextureObj> newOutsobjs;
    std::vector<TextureObj> newInTexobjs;

    std::vector<Model> useModels;
    Camera* useCamera;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init();
    void execute();

    void createShaderProgram();

    void loadViewProjMatricies (Camera cam);
    void loadModelMatricies(glm::mat4 modelTransform, bool useModelArray = false, std::vector<Model> modelArray = {});

    void createTextures();

    void textureUniforms();

    void depthBufferSetup();
    void drawBuffers();

    void bindTextures();
    void clear();

    // Maybe consolidate into an init ()
    // init shaders textures 



    // i'll probably need multiple computes

private:

    unsigned int vertex;
    unsigned int fragment;
    unsigned int program;
    unsigned int compute;

    bool checkTextures();

};







#endif