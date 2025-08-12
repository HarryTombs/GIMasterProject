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

float sdSphere( vec3 p, float s) 
{
    return length(p)-s;
};

float sdBox( vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
};

float sdScene(vec3 p)
{
    float dist = 1e9;
    for (int i =0; i < primitives.length(); i++)
    {
        vec3 localP = p - primitives[i].pos;
        if (primitives[i].type == 0) // Box;
        {
            dist = min(dist, sdBox(localP,primitives[i].size));
        }
        else if (primitives[i].type == 1) // Sphere
        {
            dist = min(dist, sdSphere(localP, primitives[i].radius));
        }
    }
    return dist;
}

uniform ivec2 Resolution;

void main()
{
    
}