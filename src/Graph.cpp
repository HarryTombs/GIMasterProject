#include "Graph.h"
#include "Pass.h"




void Graph::initGraph(const std::string& path,Scene scene)
{
    readJson(path);
    createTextures();

    sceneModels = scene.Meshes;
    sceneLights = scene.Lights;
    sceneProbes = scene.probes;
    
    for (const auto& p : passes)
    {
        
        if(p->isScreenQuad != true)
        {
            p->frameBuffer.bind();
            p->attachOutputTextures(this);
            
            // p->depthBufferSetup();

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) 
            {
                std::cerr << "Framebuffer not complete in " << p->name << ": " << status << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            CheckGLError("FrameBuffer");
        }
        if(p->isScreenQuad == true)
        {
            p->textureUniforms();
            setInt(p->shaderProgram,"numProbes", scene.probes.size());
            std::cout<<"Load uniforms pass: " << p->name << std::endl;
            if (!p->Display)
            {
                p->frameBuffer.bind();
                p->attachOutputTextures(this);
            }
            

            CheckGLError("ScreenQuad");
        }
    }
}

void Graph::mainLoop()
{
    clearBuffers();
    executePasses();
}
void Graph::clearBuffers(float r, float g, float b, float a)
{
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::string readFileToString (const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "failed to open file: " << path << std::endl;
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Graph::readJson(const std::string& path)
{
    std::string fullpath = (std::string(ASSET_DIR) + "/" + path);
    std::string json  = readFileToString(fullpath);

    Document d;
    d.Parse(json.c_str());

    if (d.HasParseError())
    {
        std::cerr << "JSON parse error: " << d.GetParseError() << " at offset " << d.GetErrorOffset() << std::endl;
        return;
    }

    if (d.HasMember("Graph") && d["Graph"].IsObject())
    {
        const auto& graph = d["Graph"];
        
        if (graph.HasMember("Passes") && graph["Passes"].IsArray())
        {
            const auto& passesArray = graph["Passes"];

            for (const auto& passVal : passesArray.GetArray()) 
            {
                std::unique_ptr<Pass> newPass = std::make_unique<Pass>(this);
                newPass->init(passVal);
                passes.push_back(std::move(newPass));
            }
            
        }
    }

    
}

void Graph::executePasses()
{
    for (const auto& p : passes)
    {
        if (!p->Display)
        {
            p->frameBuffer.bind();
        }
        
        p->clear();
        p->loadViewProjMatricies();
        for(int i = 0; i < p->Inputs.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D,textures[p->Inputs[i].name].texID);
        }
        
        if(!p->isScreenQuad)
        {
            for (Model m : sceneModels)
            {
                
                setMat4(p->shaderProgram, "model", m.transMat);
                m.Draw();
            }
            glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if(p->isScreenQuad)
        {
            glDisable(GL_DEPTH_TEST);
            renderQuad();
            glEnable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if(p->useLights)
        {
            for (unsigned int i = 0; i < sceneLights.size(); i++)
            {
                setVec3(p->shaderProgram,("lights[" + std::to_string(i) + "].Position"), sceneLights[i].pos);
                setVec3(p->shaderProgram,("lights[" + std::to_string(i) + "].Color"), sceneLights[i].col);
                setFloat(p->shaderProgram,("lights[" + std::to_string(i) + "].Linear"), sceneLights[i].linear);
                setFloat(p->shaderProgram,("lights[" + std::to_string(i) + "].Quadratic"), sceneLights[i].quadratic);
                setFloat(p->shaderProgram,("lights[" + std::to_string(i) + "].Cutoff"), sceneLights[i].cutoff);
                setVec3(p->shaderProgram,("lights[" + std::to_string(i) + "].Direction"), sceneLights[i].direction);
            }
            setVec3(p->shaderProgram,"viewPos", currentCam->CamPos);
            CheckGLError("Light creation");
        }
    }
}

void Graph::createTextures()
{
    for (const auto& p : passes)
    {
        for (TextureConfig texconf : p->Inputs)
        {
            if (textures.find(texconf.name) == textures.end())
            {
                TextureFormat newFmt;

                newFmt.internalFormat = texconf.internalFormat;
                newFmt.format = texconf.format;
                newFmt.type = texconf.type;

                TextureObj newTex;

                if(texconf.isImageTex)
                {
                    newTex.create(texconf.name,texconf.width,texconf.height,newFmt,texconf.attachmentPoint,false,texconf.imageFile,texconf.isImageTex);
                    textures[texconf.name] = newTex;
                }
                else
                {
                    newTex.create(texconf.name,texconf.width,texconf.height,newFmt,texconf.attachmentPoint);
                    textures[texconf.name] = newTex;
                }

                

                CheckGLError("TextureCreation");

                std::cout << "Made texture: " << texconf.name <<  std::endl;
            }
        }
        for (TextureConfig texconf : p->Outputs)
        {
            if (textures.find(texconf.name) == textures.end())
            {
                TextureFormat newFmt;

                newFmt.internalFormat = texconf.internalFormat;
                newFmt.format = texconf.format;
                newFmt.type = texconf.type;

                TextureObj newTex;

                newTex.create(texconf.name,texconf.width,texconf.height,newFmt,texconf.attachmentPoint);
                textures[texconf.name] = newTex;

                CheckGLError("TextureCreation");

                std::cout << "Made texture: " << texconf.name <<  std::endl;
            }
        }

    }
    
}

void Graph::resizeTextures()
{
    for (const auto& p : passes)
    {
        if(!p->isScreenQuad)
        {
            for (TextureConfig texconf : p->Outputs)
            {
                glDeleteTextures(1,&textures[texconf.name].texID);
                
                TextureFormat newFmt;

                newFmt.internalFormat = texconf.internalFormat;
                newFmt.format = texconf.format;
                newFmt.type = texconf.type;

                TextureObj newTex;

                newTex.create(texconf.name,texconf.width,texconf.height,newFmt,texconf.attachmentPoint);
                textures[texconf.name] = newTex;

                CheckGLError("TextureRecreation");

                std::cout << "Made texture: " << texconf.name <<  std::endl;

                p->frameBuffer.bind();
                p->OutAttachments.clear();
                p->attachOutputTextures(this);
                
                glBindFramebuffer(GL_FRAMEBUFFER,0);

            }
        }
        if(p->isScreenQuad)
        {
            p->textureUniforms();
        }
        
    }
}



TextureObj Graph::getTexture(const std::string& name)
{
    auto it = textures.find(name);
    return (it != textures.end()) ? &it->second : nullptr;
}