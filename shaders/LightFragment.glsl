#version 430 core
in vec2 uv;
out vec4 color;

uniform sampler2D gPos;
uniform sampler2D gNorm;
uniform sampler2D gAlbedoSpec;



void main() 
    {
        vec4 fragPos = texture(gPos, uv);
        vec3 Normal = texture(gNorm, uv).xyz;
        vec3 Abledo = texture(gAlbedoSpec, uv).xyz;
        color = vec4(fragPos.xyz,1.0);
    }