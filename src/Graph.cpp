#include "Graph.h"
#include "Pass.h"
#include "imgui.h"




void Graph::initGraph(const std::string& path,Scene scene)
{
    // Json reading and texture creation first thing
    readJson(path);
    createTextures();

    // inherit from scene graph

    sceneModels = scene.Meshes;
    sceneLights = scene.Lights;
    sceneProbes = scene.probes;
    
    // executing pass setup per pass

    for (const auto& p : passes)
    {
        
        if(p->isScreenQuad != true)
        {
            p->frameBuffer.bind();
            p->attachOutputTextures(this);
            
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) 
            {
                std::cerr << "Framebuffer not complete in " << p->name << ": " << status << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            CheckGLError("FrameBuffer");
        }
        // slight vairations in approach depending on if pass is based on the geometry or reading from textures
        if(p->isScreenQuad == true)
        {
            p->textureUniforms();
            setInt(p->shaderProgram,"numProbes", scene.probes.size());
            std::cout<<"Load uniforms pass: " << p->name << std::endl;

            p->frameBuffer.bind();
            p->attachOutputTextures(this);
            

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
    // Reading Json
    std::string fullpath = (std::string(ASSET_DIR) + "/" + path);
    std::string json  = readFileToString(fullpath);

    Document d;
    d.Parse(json.c_str());

    // Error checking
    if (d.HasParseError())
    {
        std::cerr << "JSON parse error: " << d.GetParseError() << " at offset " << d.GetErrorOffset() << std::endl;
        return;
    }

    if (d.HasMember("Graph") && d["Graph"].IsObject())
    {
        const auto& graph = d["Graph"];
        
        // getting initial details for passes then main json reading is done in pass creation

        if (graph.HasMember("Passes") && graph["Passes"].IsArray())
        {
            const auto& passesArray = graph["Passes"];

            // creating a pass for each pass value
            for (const auto& passVal : passesArray.GetArray()) 
            {
                std::unique_ptr<Pass> newPass = std::make_unique<Pass>(this);
                newPass->init(passVal);
                passes.push_back(std::move(newPass));
            }
            
        }
    }

    
}

// Render loop execution 
void Graph::executePasses()
{
    ImGui::NewFrame();

    ImGui::Begin("Change Pass");
    for (const auto& p : passes)
    {
        ImGui::Checkbox(p->name.c_str(), &p->Display);
        // If labeled as display writing to default frame buffer to show on screen 
        if (!p->Display)
        {
            p->frameBuffer.bind();
        }
        // Needed by or doesn't affect all passes
        p->clear();
        p->loadViewProjMatricies();
        for(int i = 0; i < p->Inputs.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D,textures[p->Inputs[i].name].texID);
        }
        
        // Draw call for Geometry based passes
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
        // Draw call for screen quad based passes
        if(p->isScreenQuad)
        {
            glDisable(GL_DEPTH_TEST);
            renderQuad();
            glEnable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // needed for light calculation
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
    ImGui::End();
}

void Graph::createTextures()
{
    // after pass reads JSON create textures based on read vairables
    for (const auto& p : passes)
    {
        // checks for all textures called by all passes
        for (TextureConfig texconf : p->Inputs)
        {
            // checks if texture has already been created
            if (textures.find(texconf.name) == textures.end())
            {
                // if not found new texture is created
                TextureFormat newFmt;

                newFmt.internalFormat = texconf.internalFormat;
                newFmt.format = texconf.format;
                newFmt.type = texconf.type;

                TextureObj newTex;

                // only checking image tex for inputs as outputs would not have inputs

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
            // name check if exists
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
    // resize causes texture remake
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