#ifndef GRAPH_H
#define GRAPH_H

#include "Pass.h"
#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

using namespace rapidjson;


struct Graph
{
    std::vector<std::unique_ptr<Pass>> passes;
    std::unordered_map<std::string, TextureObj> textures;
    Camera* currentCam;
    std::vector<Model> sceneModels;
    float deltaTime;

    void initGraph();
    void clearBuffers(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0);
    void readJson(const char* json);
    void executePasses();
    void getTexture(const std::string& name);
};

#endif