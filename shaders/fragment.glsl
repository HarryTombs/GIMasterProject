#version 430 core

in vec2 uv;
out vec4 color;

uniform sampler2D useTexture;

void main() 
    {
        color = texture(useTexture,uv);
    }