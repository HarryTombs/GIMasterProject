#version 430 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D GPos;
uniform sampler2D GNorm;
uniform sampler2D GAlbeSpec;
uniform sampler2D GProbeIndex;

struct Light
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;

    float Cutoff;
    vec3 Direction;
};
const int NR_Lights = 32;
uniform Light lights[NR_Lights];
uniform vec3 viewPos;

layout(std430, binding = 3) buffer IndirectResultBuffer
{
    vec3 indirectLight[];
};



void main() 
    {
        vec3 fragPos = texture(GPos, uv).xyz;
        vec3 Normal = texture(GNorm, uv).xyz;
        vec3 Abledo = texture(GAlbeSpec, uv).xyz;

        vec3 lighting = Abledo * 0.1;
        vec3 viewDir = normalize(viewPos - fragPos);

        for (int i = 0; i < NR_Lights; i++)
        {
            vec3 lightDir = normalize(lights[i].Position - fragPos);
            float theta = dot(lightDir, normalize(lights[i].Direction));

            if (theta > lights[i].Cutoff)
            {
                float diff = max(dot(Normal,lightDir), 0.0);
                float distance = length(lights[i].Position - fragPos);
                float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

                vec3 diffuse = diff * Abledo * lights[i].Color * attenuation;
                
                lighting += diffuse;
            }
        }
        int probeIndex = int(texelFetch(GProbeIndex, ivec2(gl_FragCoord.xy),0).r);

        if (probeIndex >= 0) 
        {
            vec3 indirect = indirectLight[probeIndex] * Abledo;
            lighting += indirect;
        }

        FragColor = vec4(lighting,1.0);
    }