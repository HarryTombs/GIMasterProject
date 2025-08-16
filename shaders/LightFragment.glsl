#version 430 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D GPos;
uniform sampler2D GNorm;
uniform sampler2D GAlbeSpec;
uniform sampler2D DepthTexture;

struct Light
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;

    float Cutoff;
    vec3 Direction;
};

struct Probe
{
    vec3 pos;
    float pad1;
    vec3 col;
    float pad2;
};

const int NR_Lights = 32;
uniform Light lights[NR_Lights];
uniform vec3 viewPos;
uniform int numProbes;
uniform mat4 invView;
uniform mat4 invProjection;


layout(std430, binding = 1) buffer ProbeBuffer
{
    Probe probes[];
};

layout(std430, binding = 3) buffer IndirectResultBuffer
{
    vec3 indirectLight[];
};


vec3 reconstructWorldPos(vec2 uv)
{
    float Depth = texture(DepthTexture,uv).r;
    vec4 clip = vec4(uv * 2.0 - 1.0, Depth * 2.0 - 1.0, 1.0);

    vec4 viewPos = invProjection * clip;
    viewPos /= viewPos.w;

    vec4 worldPos = invView * viewPos;
    return worldPos.xyz;
};

void main() 
    {
        vec3 fragPos = texture(GPos, uv).xyz;
        vec3 Normal = texture(GNorm, uv).xyz;
        vec3 Abledo = texture(GAlbeSpec, uv).xyz;
        float Depth = texture(DepthTexture,uv).r;

        vec3 lighting = Abledo * 0.0;
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

        int nearest = -1;
        float nearestDist = 1e20;
        for (int i = 0; i < numProbes; i++)
        {
            float dist = distance(fragPos, probes[i].pos);
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearest = i;
            }
        }

        float maxDist = 10.0; // or scene bounding box size
        vec3 nearestPos = probes[nearest].pos;
        vec3 diff = fragPos - nearestPos;
        vec3 proCol = vec3(
            clamp(diff.x / maxDist, 0.0, 1.0),
            clamp(diff.y / maxDist, 0.0, 1.0),
            clamp(diff.z / maxDist, 0.0, 1.0)
        );

        vec3 indirect = vec3(0.0);

        if (nearest >= 0) 
        {
            // indirect = indirectLight[nearest] * Abledo;
            lighting += indirect;
        };

        int numSamples = 256;

        for (int i = 0; i < numSamples; i++)
        {
            vec2 rand = vec2(fract(sin(dot(uv, vec2(12.9898,78.233))) * 43758.5453 + float(i)));
            float angle = rand.x * 6.2831;
            float radius = rand.y * 0.1;
            vec2 offsetUV = uv + radius * vec2(cos(angle), sin(angle));

            vec3 neighborPos = reconstructWorldPos(offsetUV);
            vec3 nieghborCol = texture(GAlbeSpec, offsetUV).rbg;

            if (dot(neighborPos - fragPos, Normal) > 0.0)
            {
                indirect += nieghborCol;
            }
        }

        indirect /= float(numSamples);
        lighting += indirect;

        FragColor = vec4(lighting,1.0);
    }