#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include <vector>
#include <GL/glew.h>
#include <string>


class Pass
{
public:

    Pass(std::string vertpath, std::string fragpath, bool screenQuad = false)
    {
        vert = vertpath;
        frag = fragpath;
        isScreenQuad = screenQuad;
    };
    std::vector<TextureObj> In;
    std::vector<TextureObj> Out;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init()
    {
        createShaderProgram();
    }
    void execute();

    void createShaderProgram()
    {
        unsigned int prog = loadShaderProgram(vert, frag);
        shaderProgram = prog;
    }
    // Maybe consolidate into an init ()
    // init shaders textures 



    // i'll probably need multiple computes

private:

    unsigned int vertex;
    unsigned int fragment;
    unsigned int program;
    unsigned int compute;

    


    


    bool checkTextures();
    void bindTextures()
    {
        int i = 0;
        for (TextureObj tex : In)
        {
            //Textures need to be created before this
            frameBuffer.attachTexture(tex);
            i++;
        }
    }

    
    

};

struct Graph
{
    std::vector<Pass> Passes;
    float deltaTime;

    void executePasses()
    {
        for (Pass p : Passes)
        {
            p.frameBuffer.bind();
        }
    };
};

GLuint quadVAO,quadVBO;

void createScreenQuad()
{
    
    float screenQuadVerticies[] = 
        {
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
        };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(screenQuadVerticies),&screenQuadVerticies,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)(3*sizeof(float)));

}

void renderQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


#endif