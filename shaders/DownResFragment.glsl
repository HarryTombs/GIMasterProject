#version 430 core
in vec2 uv;
out vec3 DownResTexture;

uniform sampler2D SSGITexture;
uniform vec2 texelSize;

void main() 
{
    vec3 col = vec3(0.0);
    float count = 0.0;

    for (int x = 0; x < 2; x++)
    {
        for (int y =0; y < 2; y++)
        {
            vec2 offset = vec2(x,y) * texelSize;
            col += texture(SSGITexture, uv + offset).rgb;
            count += 1.0;
        }
    }
    DownResTexture = col / count;
}