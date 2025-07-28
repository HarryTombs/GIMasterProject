#include "Graph.h"

void Graph::initGraph()
{
    // Passes[0].In[0].create(ScreenWidth,ScreenHeight,Passes[0].InFmt[0],GL_COLOR_ATTACHMENT0,true);
}
void Graph::clearBuffers(float r, float g, float b, float a)
{
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// pass 1 create input textures then for each check if texture exists

void Graph::readJson(const char* json)
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
}

void getTexture(const std::string& name)
{
    
}