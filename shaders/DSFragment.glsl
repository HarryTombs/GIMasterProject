#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D tex;

layout (location=0)out vec3 gPosition;
layout (location=1)out vec3 gNormal;
layout (location=2)out vec4 gAlbedoSpec;



void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = texture(tex,TexCoords).rgb;
    gAlbedoSpec.a = 1.0;
}