#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include "ShaderUtils.h"
#include "SDLWindow.h"
#include "Camera.h"
#include "Mesh.h"
#include "Graph.h"
#include <vector>
#include <GL/glew.h>
#include <string>
#include <rapidjson/document.h>

using namespace rapidjson;

class Pass
{
public:

    Pass(std::string vertpath, std::string fragpath, Camera cam, std::vector<Model> models = {}, bool screenQuad = false,
        const std::vector<std::string>& texturesIn = {}, const std::vector<std::string>& texturesOut = {},
        const std::vector<TextureFormat> formatIn = {},const std::vector<TextureFormat> formatOut = {});

    // Graph* graph;

    std::vector<std::string> In;
    std::vector<std::string> Out;
    std::vector<TextureFormat> InFmt;
    std::vector<TextureFormat> OutFmt;

    std::vector<GLenum> attachments;

    std::vector<TextureObj> newOutsobjs;
    std::vector<TextureObj> newInTexobjs;

    std::vector<Model> useModels;
    // Camera* useCamera = graph->currentCam;
    FrameBufferObject frameBuffer;

    bool isScreenQuad;

    unsigned int shaderProgram;

    std::string vert;
    std::string frag;
    void init();
    void execute()
    {
        frameBuffer.bind();
        clear();
        // for(int i = 0; i < In.size(); i++)
        // {
        //     glActiveTexture(GL_TEXTURE0 + i);
        //     glBindTexture(GL_TEXTURE_2D, newInTexobjs[i].texID);
        // }

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
        bool texExists = false;
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







#endif