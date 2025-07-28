#include "Graph.h"
#include "Pass.h"

void Graph::initGraph(const std::string& path)
{
    readJson(path);
    createTextures();
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
        std::cout << "I've found graph" << std::endl;
        
        if (graph.HasMember("Passes") && graph["Passes"].IsArray())
        {
            const auto& passesArray = graph["Passes"];
            std::cout << "I've found passes" << std::endl;

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
    // for (Pass p : Passes)
//     {
//         p.frameBuffer.bind();
//     }
}

void Graph::createTextures()
{
    for (const auto& p : passes)
    {
        for (std::string texName : p->InputNames)
        {
            TextureFormat fmt;
            GLenum formatss = GL_RGBA;
            GLenum fors = GL_FLOAT;
            fmt.format = formatss;
            fmt.internalFormat = formatss;
            fmt.type =  fors;

            // Load these properly please it'll work :) 

            if (textures.find(texName) == textures.end())
            {
                TextureFormat newFmt;

                newFmt.internalFormat = fmt.internalFormat;
                newFmt.format = fmt.format;
                newFmt.type = fmt.type;

                TextureObj newTex;

                newTex.create(texName,ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0);
                textures[texName] = newTex;

                std::cout << "Made texture: " << texName << std::endl;
            }
        }
        for (std::string texName : p->OutputNames)
        {
            TextureFormat fmt;
            GLenum formatss = GL_RGBA;
            GLenum fors = GL_FLOAT;
            fmt.format = formatss;
            fmt.internalFormat = formatss;
            fmt.type =  fors;

            // Load these properly please it'll work :) 

            if (textures.find(texName) == textures.end())
            {
                TextureFormat newFmt;

                newFmt.internalFormat = fmt.internalFormat;
                newFmt.format = fmt.format;
                newFmt.type = fmt.type;

                TextureObj newTex;

                newTex.create(texName,ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0);
                textures[texName] = newTex;

                std::cout << "Made texture: " << texName << std::endl;
            }
        }

    }

    // find the name and format from json !!!!!

    // make this a for loop add i to attachment0

    
}

TextureObj Graph::getTexture(const std::string& name)
{
    auto it = textures.find(name);
    return (it != textures.end()) ? &it->second : nullptr;
}