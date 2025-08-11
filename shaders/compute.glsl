#version 430

layout(local_size_x = 16, local_size_y = 16) in;

struct SDFPrimitive 
{
    int type;        // 4 bytes
    vec3 pos;        // 12 bytes (16 byte aligned)
    vec3 size;       // 12 bytes (16 byte aligned)
    vec3 rotation;   // 12 bytes (16 byte aligned)
    float radius;    // 4 bytes
    float pad1;      // padding
    float pad2;      // padding
    float pad3;      // padding
};

layout(std430, binding = 0 ) buffer SDFBuffer 
{
    SDFPrimitive primitives[];
};

uniform ivec2 Resolution;

void main()
{
    
}