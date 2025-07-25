#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include "SDLWindow.h"
#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <GL/glew.h>
#include <string>


class Pass
{
public:

    Pass(std::string vertpath, std::string fragpath, Camera cam, std::vector<Model> models = {}, bool screenQuad = false,
        const std::vector<std::string>& texturesIn = {}, const std::vector<std::string>& texturesOut = {},
        const std::vector<TextureFormat> formatIn = {},const std::vector<TextureFormat> formatOut = {})
    {
        vert = vertpath;
        frag = fragpath;
        isScreenQuad = screenQuad;
        In = texturesIn;
        Out = texturesOut;
        InFmt = formatIn;
        OutFmt = formatOut;
        useCamera = cam;
        useModels = models;
    };
    std::vector<std::string> In;
    std::vector<std::string> Out;
    std::vector<TextureFormat> InFmt;
    std::vector<TextureFormat> OutFmt;

    std::vector<GLenum> attachments;

    std::vector<TextureObj> newOutsobjs;
    std::vector<TextureObj> newInTexobjs;

    std::vector<Model> useModels;
    Camera useCamera;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init()
    {
        createShaderProgram();
        frameBuffer.create();
        createTextures();
        textureUniforms();
    }
    void execute()
    {
        frameBuffer.bind();
        clear();
        for(int i = 0; i < In.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, newInTexobjs[i].texID);
        }

        // if (!isScreenQuad)
        // {
        //     loadViewProjMatricies(useCamera);  
        //     for (Model m : useModels)
        //     {
        //         loadModelMatricies(m.transMat);
        //         m.Draw();

        //     } 
        // }
    }

    void createShaderProgram()
    {
        unsigned int prog = loadShaderProgram(vert, frag);
        shaderProgram = prog;
    }

    void loadViewProjMatricies (Camera cam) 
    {
        glUseProgram(shaderProgram);

        glm::mat4 view = cam.getView();
        glm::mat4 projection = glm::perspective(glm::radians(cam.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);
        setMat4(shaderProgram, "view", view);
        setMat4(shaderProgram, "projection", projection);
    }

    void loadModelMatricies(glm::mat4 modelTransform, bool useModelArray = false, std::vector<Model> modelArray = {})
    {
        setMat4(shaderProgram, "model", modelTransform);
        if (useModelArray)
        {
            for (Model m : modelArray)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model,m.pos);
                // do rotations
                model = glm::scale(model,m.scale);
            }
        }
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
            newInTexobjs = newTexObjs;
        }
        else 
        {

        }

        std::vector<TextureObj> newOuts;
        std::vector<GLenum> newAttachments;
        for ( int i = 0; i < Out.size(); i++)
        {
            TextureFormat newFmt;

            newFmt.internalFormat = OutFmt[i].internalFormat;
            newFmt.format = OutFmt[i].format;
            newFmt.type = OutFmt[i].type;

            TextureObj newTex;

            newTex.create(Out[i],ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0 + i);
            newAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
            newOuts.push_back(newTex);
            frameBuffer.bind();
            frameBuffer.attachTexture(newTex);
        }
        newOutsobjs = newOuts;
        attachments = newAttachments;
        CheckGLError("Pass create textureobj");
    }

    void textureUniforms()
    {
        glUseProgram(shaderProgram);
        for(int i = 0; i < In.size(); i++)
        {
            setInt(shaderProgram,In[i],i);
        }
    }

    void depthBufferSetup()
    {
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);   
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScreenWidth , ScreenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawBuffers()
    {
        glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
    }

    void bindTextures()
    {

        for (int i = 0; i < Out.size(); i++)
        {
            frameBuffer.attachTexture(newOutsobjs[i]);
        }
        CheckGLError("BindingTextures");
    }

    void clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    void clearBuffers(double r = 0.0, double g = 0.0, double b = 0.0, double a = 1.0)
    {
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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