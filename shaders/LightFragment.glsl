#version 430 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNorm;
uniform sampler2D gAlbedoSpec;



void main() 
    {
        vec3 fragPos = texture(gPosition, uv).xyz;
        vec3 Normal = texture(gNorm, uv).xyz;
        vec3 Abledo = texture(gAlbedoSpec, uv).xyz;
        FragColor = vec4(Abledo,1.0);
    }