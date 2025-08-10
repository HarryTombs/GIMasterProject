#version 430

layout(local_size_x = 16, local_size_y = 16) in;

struct SDFPrimitive 
{
    int type;
    vec3 pos;
    float radius;
};

layout(std430, binding = 0 ) buffer SDFBuffer 
{
    SDFPrimitive primitives[];
};

uniform ivec2 Resolution;

void main()
{
    
}