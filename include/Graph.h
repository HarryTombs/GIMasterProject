#ifndef GRAPH_H
#define GRAPH_H

#include "Pass.h"
#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <GL/glew.h>
#include <string>
#include <rapidjson/document.h>

using namespace rapidjson;


struct Graph
{
    std::vector<std::unique_ptr<Pass>> passes;
    Camera* currentCam;
    std::vector<Model> sceneModels;
    float deltaTime;

    void initGraph()
    {
        // Passes[0].In[0].create(ScreenWidth,ScreenHeight,Passes[0].InFmt[0],GL_COLOR_ATTACHMENT0,true);
    }
    void clearBuffers(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0)
    {
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // pass 1 create input textures then for each check if texture exists

    void readJson(const char* json)
    {
        Document d;
        d.Parse(json);
    }

    void executePasses()
    {
        // for (Pass p : Passes)
    //     {
    //         p.frameBuffer.bind();
    //     }
    };
};

#endif