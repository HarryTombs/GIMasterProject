#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include "TextureUtils.h"
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

    std::vector<TextureConfig> Inputs;
    std::vector<TextureConfig> Outputs;

    std::vector<GLenum> InAttachments;
    std::vector<GLenum> OutAttachments;

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

    void loadViewProjMatricies(Camera cam);
    void loadModelMatricies();

    void createTextures();

    void textureUniforms();
    void attachOutputTextures(Graph* graph);

    void depthBufferSetup();
    void drawBuffers();

    void bindTextures();
    void clear();


private:

    unsigned int vertex;
    unsigned int fragment;
    unsigned int program;
    unsigned int compute;

    bool checkTextures();

};







#endif