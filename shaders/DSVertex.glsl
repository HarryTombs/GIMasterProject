#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 MVP;
uniform mat3 normalMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
vec4 worldPos;


void main()
{
    worldPos = Model * vec4(aPos, 1.0);
    Normal = normalMatrix*aNormal;
    TexCoord = aUV.st;
    FragPos = worldPos.xyz;
    gl_Position = Projection * View * worldPos;
}