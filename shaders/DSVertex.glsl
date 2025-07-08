#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() 
{
    vec4 worldPos = model * vec4(pos,1.0);
    FragPos = worldPos.xyz;
    TexCoords = texCoord;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * normal;

    gl_Position = projection * view * model * vec4(pos, 1.0);

}