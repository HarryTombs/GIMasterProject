#ifndef PASS_H
#define PASS_H
#include "FrameBufferObject.h"
#include <vector>
#include <GL/glew.h>


class Pass 
{
public:
    std::vector<TextureObj> In;
    std::vector<TextureObj> Out;
    FrameBufferObject frameBuffer;

    void execute();


    // i'll probably need multiple computes

private:

    unsigned int vertex;
    unsigned int fragment;
    unsigned int compute;
    void bindTextures()
    {
        int i = 0;
        for (TextureObj tex : In)
        {
            // active textures???
            frameBuffer.attachTexture(tex);
            i++;
        }
    }

};

struct Graph
{
    std::vector<Pass> Passes;

    void executePasses()
    {
        for (Pass p : Passes)
        {
            p.frameBuffer.bind();
        }
    };
};


#endif