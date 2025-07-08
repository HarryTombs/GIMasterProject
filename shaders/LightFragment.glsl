#version 430 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNorm;
uniform sampler2D gAlbedoSpec;

struct Light
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};
const int NR_Lights = 32;
uniform Light lights[NR_Lights];
uniform vec3 viewPos;



void main() 
    {
        vec3 fragPos = texture(gPosition, uv).xyz;
        vec3 Normal = texture(gNorm, uv).xyz;
        vec3 Abledo = texture(gAlbedoSpec, uv).xyz;

        vec3 lighting = Abledo * 0.1;
        vec3 viewDir = normalize(viewPos - fragPos);
        for (int i = 0; i < NR_Lights; i++)
        {
            vec3 lightDir = normalize(lights[i].Position - fragPos);
            Abledo = max(dot(Normal,lightDir), 0.0) * Abledo * lights[i].Color;

            float distance = length(lights[i].Position - fragPos);
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            Abledo *= attenuation;
            lighting += Abledo;
        }
        FragColor = vec4(lighting,1.0);
    }