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
struct Graph;

class Pass
{
public:

    Pass(Graph* parentGraph);

    Graph* graph = nullptr;

    std::string name;

    std::vector<std::string> InputNames;
    std::vector<std::string> OutputNames;

    std::vector<GLenum> inAttachments;
    std::vector<GLenum> outAttachments;

    std::vector<TextureFormat> inFmts;
    std::vector<TextureFormat> outFmts;

    std::vector<Model> useModels;
    Camera* useCamera;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init(const rapidjson::Value& passJson);
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