#include "Graph.h"
#include "Pass.h"

void Graph::initGraph(const std::string& path)
{
    readJson(path);
    createTextures();
    
    for (const auto& p : passes)
    {
        
        if(p->isScreenQuad != true)
        {
            p->frameBuffer.bind();
            p->attachOutputTextures(this);
            
            p->depthBufferSetup();

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) 
            {
                std::cerr << "Framebuffer not complete in " << p->name << ": " << status << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if(p->isScreenQuad == true)
        {
            p->textureUniforms();
            std::cout<<"Load uniforms pass: " << p->name << std::endl;
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
        if (!p->isScreenQuad)
        {
            p->frameBuffer.bind();
            p->clear();
            
            glUseProgram(p->shaderProgram);

            if (!glIsProgram(p->shaderProgram)) {
                std::cerr << "Invalid shader program in pass: " << p->name << std::endl;
            }
            for(int i = 0; i < p->Inputs.size(); i++)
            {
                if (textures.find(p->Inputs[i].name) != textures.end())
                {
                    TextureObj tex = getTexture(p->Inputs[i].name);
                    std::cout << "Attaching Texture: " << p->Inputs[i].name << " To: " << p->name << " At: " << GL_TEXTURE0+i <<std::endl;
                    glActiveTexture(GL_TEXTURE0+i);

                    if (tex.texID != 0)
                    {
                        glBindTexture(GL_TEXTURE_2D, tex.texID);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                        // Set uniform
                        GLint loc = glGetUniformLocation(p->shaderProgram, "tex");
                        if (loc != -1)
                        {
                            glUniform1i(loc, i);  // Link shader uniform `tex` to unit i
                            std::cout << "Uniform location set: " << i << std::endl;
                        }
                        else
                        {
                            std::cerr << "Could not find uniform 'tex' in shader " << p->name << std::endl;
                        }
                    }

                    else std::cerr << "Invalid texture ID for: " << p->Inputs[i].name << std::endl;
                }
                
                
            }
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "FBO incomplete in pass " << p->name << ": 0x" << std::hex << status << std::endl;

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

                newTex.create(texconf.name,texconf.width,texconf.height,newFmt,texconf.attachmentPoint);
                textures[texconf.name] = newTex;

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



TextureObj Graph::getTexture(const std::string& name)
{
    auto it = textures.find(name);
    return (it != textures.end()) ? &it->second : nullptr;
}