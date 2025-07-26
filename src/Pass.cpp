#include "Pass.h"


Pass::Pass(std::string vertpath, std::string fragpath, Camera cam, std::vector<Model> models, bool screenQuad,
        const std::vector<std::string>& texturesIn, const std::vector<std::string>& texturesOut,
        const std::vector<TextureFormat> formatIn,const std::vector<TextureFormat> formatOut)
        {
            vert = vertpath;
            frag = fragpath;
            isScreenQuad = screenQuad;
            In = texturesIn;
            Out = texturesOut;
            InFmt = formatIn;
            OutFmt = formatOut;
            useModels = models;
        }

void Pass::init()
    {
        createShaderProgram();
        frameBuffer.create();
        createTextures();
        textureUniforms();
    }