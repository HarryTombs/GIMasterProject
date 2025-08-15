#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;
uniform mat4 model;

layout (location=0)out vec3 GPos;
layout (location=1)out vec3 GNorm;
layout (location=2)out vec4 GAlbeSpec;
layout (location=3)out int GProbeIndex;

struct Probe
{
    vec3 pos;
    float pad1;
    vec3 col;
    float pad2;
};

layout(std430, binding = 1) buffer ProbeBuffer
{
    Probe probes[];
};

void main()
{
    GPos = FragPos;
    GNorm = normalize(Normal);
    GAlbeSpec.rgb = texture(tex,TexCoords).rgb;
    GAlbeSpec.a = 1.0;

    int nearest = -1;
    float nearestDist = 1e20; 

    for(int i=0; i < 864; i++ )
    {
        float dist = distance(FragPos, (model * vec4(probes[i].pos,1.0)).rgb);
        if (dist < nearestDist)
        {
            nearestDist = dist;
            nearest = i;
        }
    }
    GProbeIndex = nearest;

}
