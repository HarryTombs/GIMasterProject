#include "Pass.h"
#include "Graph.h"


Pass::Pass(Graph* parentGraph) : graph(parentGraph){}

void Pass::init(const rapidjson::Value& passJson)
    {
        // main JSON reading function defining whole individaul pass
        if(passJson.HasMember("PassName"))
        {
            name = passJson["PassName"].GetString();
        }
        // JSON safety checks
        if (passJson.HasMember("Inputs") && passJson["Inputs"].IsArray()) 
        {
            for (const auto& input : passJson["Inputs"].GetArray()) 
            {
                // create new texconfig according to JSON definition 
                TextureConfig tex; 
                tex.name = input["name"].GetString();
                tex.existing = input["existing"].GetBool();
                if (!input["existing"].GetBool())
                {
                    tex.width = input["width"].GetInt();
                    tex.height = input["height"].GetInt();
                    GLenum attachPoint = getGLEnumFromString(input["attachmentPoint"].GetString());
                    tex.attachmentPoint = attachPoint;
                    tex.isImageTex = input["isImageTex"].GetBool();
                    if (input["isImageTex"].GetBool())
                    {
                        tex.imageFile = input["TextureFile"].GetString();
                    }

                    const auto& fmt = input["format"];
                    tex.internalFormat = getGLEnumFromString(fmt["internalFormat"].GetString());
                    tex.format = getGLEnumFromString(fmt["format"].GetString());
                    tex.type = getGLEnumFromString(fmt["type"].GetString());
                }

                Inputs.push_back(tex);
                
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
                GLenum attachPoint = getGLEnumFromString(output["attachmentPoint"].GetString());
                tex.attachmentPoint = attachPoint;
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

        // Storing Shader information
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
        // General Pass information
        if (passJson.HasMember("ScreenQuad") && passJson["ScreenQuad"].IsBool())
        {
            isScreenQuad = passJson["ScreenQuad"].GetBool();
        }
        if (passJson.HasMember("UseLights") && passJson["UseLights"].IsBool())
        {
            useLights = passJson["UseLights"].GetBool();
        }
        if (passJson.HasMember("Display") && passJson["Display"].IsBool())
        {
            Display = passJson["Display"].GetBool();
        }

        createShaderProgram();
        frameBuffer.create();

        useCamera = graph->currentCam;
        useModels = graph->sceneModels;

        
    }


void Pass::createShaderProgram()
{
    unsigned int prog = loadShaderProgram(vert, frag);
    shaderProgram = prog;
}

void Pass::loadViewProjMatricies()
{
    // make sure the camera exists
    if (!useCamera) {
        std::cerr << "useCamera is nullptr in loadViewProjMatricies()\n";
        return;
    }
    glUseProgram(shaderProgram);

    // Load all needed uniforms 
    glm::mat4 view = useCamera->getView();
    glm::mat4 projection = glm::perspective(glm::radians(useCamera->m_zoom), (float)ScreenWidth/ (float)ScreenHeight,0.01f,1000.0f);
    setMat4(shaderProgram, "view", view);
    setMat4(shaderProgram, "projection", projection);
    float timeSec = SDL_GetTicks() / 1000.0f;
    setInt(shaderProgram,"frame",frameCount);
    glm::mat4 invView = glm::inverse(view);
    glm::mat4 invProjection = glm::inverse(projection);
    setMat4(shaderProgram, "invView", invView);
    setMat4(shaderProgram, "invProjection", invProjection);
    setVec2(shaderProgram, "texelSize",(glm::vec2(1.0)/glm::vec2(ScreenHeight,ScreenWidth)));
    setVec2(shaderProgram, "Resolution",glm::vec2(ScreenWidth,ScreenHeight));
}


void Pass::textureUniforms()
{
    // loading texture uniforms
    glUseProgram(shaderProgram);
    for(int i = 0; i < Inputs.size(); i++)
    {
        setInt(shaderProgram,Inputs[i].name,i);
        
    } 
}

void Pass::drawBuffers()
{
    // Draw buffers to Framebuffer
    std::vector<GLenum> useAttchments;
    for (int i = 0; i < OutAttachments.size();i++)
    {
        if (OutAttachments[i] != GL_DEPTH_ATTACHMENT)
        {
            useAttchments.push_back(OutAttachments[i]);
        }
    }   
    glDrawBuffers(static_cast<GLsizei>(useAttchments.size()), useAttchments.data());
    std::cout << "Draw Buffers Complete " << useAttchments.size() << std::endl;
}


void Pass::attachOutputTextures(Graph* graph)
{
    // Finding texture attachment points and attaching them
    frameBuffer.bind();
    for (auto& tex : Outputs)
    {
        if (graph->textures.find(tex.name) != graph->textures.end())
        {
            GLuint texID = graph->textures[tex.name].texID;

            if (texID == 0)
            std::cerr << "Warning: texture ID 0 for " << tex.name << std::endl;


            glFramebufferTexture2D(GL_FRAMEBUFFER, tex.attachmentPoint, GL_TEXTURE_2D,texID,0);
            OutAttachments.push_back(tex.attachmentPoint);
            std::cout << "Texture: " << tex.name << " Attached at: " << tex.attachmentPoint << std::endl;
        }
    }
    drawBuffers();

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete for pass: " << name
                << " Status: 0x" << std::hex << status << std::dec << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
};

void Pass::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}