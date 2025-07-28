#include "Graph.h"

void Graph::initGraph()
{
    // Passes[0].In[0].create(ScreenWidth,ScreenHeight,Passes[0].InFmt[0],GL_COLOR_ATTACHMENT0,true);
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
            const auto& passes = graph["Passes"];
            std::cout << "I've found passes" << std::endl;

            for (const auto& passVal : passes.GetArray()) 
            {
                if (!passVal.IsObject()) continue;

                std::string passName = passVal["PassName"].GetString();
                std::string framebuffer = passVal["FrameBuffer"].GetString();

                std::cout << "Pass: " << passName << ", FBO: " << framebuffer << std::endl;
                
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
    std::string texName;
    TextureFormat fmt;

    // find the name and format from json !!!!!

    // make this a for loop add i to attachment0

    if (textures.find(texName) == textures.end())
    {
        TextureFormat newFmt;

        newFmt.internalFormat = fmt.internalFormat;
        newFmt.format = fmt.format;
        newFmt.type = fmt.type;

        TextureObj newTex;

        newTex.create(texName,ScreenWidth,ScreenHeight,newFmt,GL_COLOR_ATTACHMENT0);
        textures[texName] = newTex;
    }
}

TextureObj Graph::getTexture(const std::string& name)
{
    auto it = textures.find(name);
    return (it != textures.end()) ? &it->second : nullptr;
}