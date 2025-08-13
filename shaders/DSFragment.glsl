#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;

layout (location=0)out vec3 GPos;
layout (location=1)out vec3 GNorm;
layout (location=2)out vec4 GAlbeSpec;
layout (location=3)out vec4 GProbeIndex;



void main()
{
    GPos = FragPos;
    GNorm = normalize(Normal);
    GAlbeSpec.rgb = texture(tex,TexCoords).rgb;
    GAlbeSpec.a = 1.0;




}
