#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include "SDLWindow.h"
#include <vector>
#include <GL/glew.h>
#include <string>


class Pass
{
public:

    Pass(std::string vertpath, std::string fragpath, bool screenQuad = false,const std::vector<std::string>& texturesIn = {}, const std::vector<std::string>& texturesOut = {},const std::vector<TextureFormat> formatIn = {},const std::vector<TextureFormat> formatOut = {})
    {
        vert = vertpath;
        frag = fragpath;
        isScreenQuad = screenQuad;
        In = texturesIn;
        Out = texturesOut;
        InFmt = formatIn;
        OutFmt = formatOut;
    };
    std::vector<std::string> In;
    std::vector<std::string> Out;
    std::vector<TextureFormat> InFmt;
    std::vector<TextureFormat> OutFmt;

    std::vector<TextureObj> newOutsobjs;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init()
    {
        createShaderProgram();
        frameBuffer.create();
    }
    void execute();

    void createShaderProgram()
    {
        unsigned int prog = loadShaderProgram(vert, frag);
        shaderProgram = prog;
    }

    void createTextures()
    {
        bool texExists = true;
        // CHECK IF THEY EXIST !!!!!
        if (texExists != true)
        {
            std::vector<TextureObj> newTexObjs;
            for( int i=0; i < In.size(); i++ )
            {
                    TextureFormat newFmt;

                    newFmt.internalFormat = InFmt[i].internalFormat;
                    newFmt.format = InFmt[i].format;
                    newFmt.type = InFmt[i].type;

                    TextureObj newTex;

                    newTex.create(In[i],ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0 + i);
                    newTexObjs.push_back(newTex);

                    // just use a textureobj class until you implent the json reading
            }

        }

        std::vector<TextureObj> newOuts;

        for ( int i = 0; i < Out.size(); i++)
        {
            TextureFormat newFmt;

            newFmt.internalFormat = OutFmt[i].internalFormat;
            newFmt.format = OutFmt[i].format;
            newFmt.type = OutFmt[i].type;

            TextureObj newTex;

            // How do i get them to have different names?

            newTex.create(Out[i],ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0 + i);

            // gbuffer attach?
            newOuts.push_back(newTex);
            frameBuffer.attachTexture(newTex);

            std::cout << "TexID is: " << newTex.texID << std::endl;

            std::cout << "made Texture " << Out[i] << std::endl;
        }
        newOutsobjs = newOuts;
        CheckGLError("Pass create textureobj");
    }

    void bindTextures()
    {

        for (int i = 0; i < Out.size(); i++)
        {
            frameBuffer.attachTexture(newOutsobjs[i]);
        }
        CheckGLError("BindingTextures");
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

};

struct Graph
{
    std::vector<Pass> Passes;
    float deltaTime;

    void initGraph()
    {
        // Passes[0].In[0].create(ScreenWidth,ScreenHeight,Passes[0].InFmt[0],GL_COLOR_ATTACHMENT0,true);
    }

    // pass 1 create input textures then for each check if texture exists


    void executePasses()
    {
        for (Pass p : Passes)
        {
            p.frameBuffer.bind();
        }
    };
};

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;

void renderQuad()
{
    if (quadVAO == 0)
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
        CheckGLError("Make Quad");
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    CheckGLError("Render Quad");
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCube()
{
    if (cubeVAO == 0)
    {
        float cubeVerticies[] = 
            {
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies), &cubeVerticies, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            std::cout << "CubeMade" << std::endl;
            CheckGLError("Make Cube");
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    CheckGLError("Render Cube");
    
}


#endif