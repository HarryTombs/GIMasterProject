#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPos;
uniform sampler2D gNorm;
uniform sampler2D gColorSpec;
struct light
{
    vec3 lightsPos;
    float padding;
    vec3 lightsCol;
    float radius;
};
layout(std430, binding = 0) buffer LightBuffer{
    light lights[];
};
uniform int numLights;

uniform vec3 viewPos;
uniform float Linear;
uniform float Quadratic;


void main()
{
    vec3 Fragpos = texture(gPos, TexCoord).rgb;
    vec3 Normal = texture(gNorm, TexCoord).rgb;
    vec3 albedo = texture(gColorSpec, TexCoord).rgb;

    vec3 lighting = albedo * 0.1;
    vec3 viewDir = normalize(viewPos - Fragpos);
    for(int i = 0; i < numLights; i++)
    {
        vec3 lightpos = lights[i].lightsPos;
        vec3 lightcol = lights[i].lightsCol;
        float radius = lights[i].radius;
        float distance  = length(lightpos - Fragpos);
        if(distance < radius)
        {
            vec3 lightDir = normalize(lightpos - Fragpos);
            float diff = max(dot(Normal,lightDir), 0.0);
            vec3 diffuse = diff * albedo * lightcol;
            vec3 halfwaydir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwaydir), 0.0), 16.0);
            vec3 specular = spec * lightcol;
            float attenuiation = 1.0 / (1.0 +Linear *distance + Quadratic * distance * distance);
            diffuse *= attenuiation;
            specular *= attenuiation;

            lighting += diffuse + specular;
        }
    }
    FragColor = vec4(lighting, 1.0);
}