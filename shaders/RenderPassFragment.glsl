#version 430 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D LightTexture;

void main() 
{
    vec3 lighting = texture(LightTexture, uv).xyz;
    FragColor = vec4(lighting,1);
}