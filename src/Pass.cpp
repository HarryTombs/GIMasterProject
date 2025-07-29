#include "Pass.h"
#include "Graph.h"


Pass::Pass(Graph* parentGraph) : graph(parentGraph){}

void Pass::init(const rapidjson::Value& passJson)
    {
        if(passJson.HasMember("PassName"))
        {
            name = passJson["PassName"].GetString();
        }

        if (passJson.HasMember("Inputs") && passJson["Inputs"].IsArray()) 
        {
            for (const auto& input : passJson["Inputs"].GetArray()) 
            {
                TextureConfig tex; 
                tex.name = input["name"].GetString();
                tex.width = input["width"].GetInt();
                tex.height = input["height"].GetInt();
                tex.attachmentPoint = getGLEnumFromString(input["attachmentPoint"].GetString());
                tex.isImageTex = input["isImageTex"].GetBool();
                if (input["isImageTex"].GetBool())
                {
                    tex.imageFile = input["TextureFile"].GetString();
                }

                const auto& fmt = input["format"];
                tex.internalFormat = getGLEnumFromString(fmt["internalFormat"].GetString());
                tex.format = getGLEnumFromString(fmt["format"].GetString());
                tex.type = getGLEnumFromString(fmt["type"].GetString());

                
                Inputs.push_back(tex);
                
                // Optionally store more info like format/attachment
            }
        }

        if (passJson.HasMember("Outputs") && passJson["Outputs"].IsArray()) 
        {
            for (const auto& output : passJson["Outputs"].GetArray()) 
            {
                TextureConfig tex; 
                tex.name = output["name"].GetString();
                tex.width = output["width"].GetInt();
                tex.height = output["height"].GetInt();
                tex.attachmentPoint = getGLEnumFromString(output["attachmentPoint"].GetString());
                tex.isImageTex = output["isImageTex"].GetBool();
                if (output["isImageTex"].GetBool())
                {
                    tex.imageFile = output["TextureFile"].GetString();
                }

                const auto& fmt = output["format"];
                tex.internalFormat = getGLEnumFromString(fmt["internalFormat"].GetString());
                tex.format = getGLEnumFromString(fmt["format"].GetString());
                tex.type = getGLEnumFromString(fmt["type"].GetString());
                Outputs.push_back(tex);
            }
        }

        if (passJson.HasMember("Shaders") && passJson["Shaders"].IsObject())
        {
            const auto& shaders = passJson["Shaders"];
            if(shaders.HasMember("Vertex"))
            {
                std::string inVert = shaders["Vertex"].GetString();
                vert = inVert;
            }
            if(shaders.HasMember("Fragment"))
            {
                std::string inFrag = shaders["Fragment"].GetString();
                frag = inFrag;
            }
        }
        if (passJson.HasMember("ScreenQuad") && passJson["ScreenQuad"].IsBool())
        {
            isScreenQuad = passJson["ScreenQuad"].GetBool();
        }

        createShaderProgram();
        frameBuffer.create();
        
    }

void Pass::execute()
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

void Pass::createShaderProgram()
{
    unsigned int prog = loadShaderProgram(vert, frag);
    shaderProgram = prog;
}

void Pass::loadViewProjMatricies(Camera cam)
{
    glUseProgram(shaderProgram);

    glm::mat4 view = cam.getView();
    glm::mat4 projection = glm::perspective(glm::radians(cam.m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);
    setMat4(shaderProgram, "view", view);
    setMat4(shaderProgram, "projection", projection);
}

void Pass::loadModelMatricies(glm::mat4 modelTransform, bool useModelArray, std::vector<Model> modelArray)
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


void Pass::textureUniforms()
{
    glUseProgram(shaderProgram);
    for(int i = 0; i < Inputs.size(); i++)
    {
        setInt(shaderProgram,Inputs[i].name,i);
    }
}

void Pass::depthBufferSetup()
{
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);   
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScreenWidth , ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// void Pass::drawBuffers()
// {
//     glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
// }

// void Pass::bindTextures()
// {

//     for (int i = 0; i < OutputNames.size(); i++)
//     {
//         frameBuffer.attachTexture(newOutsobjs[i]);
//     }
//     CheckGLError("BindingTextures");
// }

void Pass::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}