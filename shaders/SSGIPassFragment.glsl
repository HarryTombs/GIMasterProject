#version 430 core
in vec2 uv;

layout (location=0)out vec3 SSGITexture;

uniform sampler2D GNorm;
uniform sampler2D LightTexture;
uniform sampler2D DepthTexture;

uniform mat4 invView;
uniform mat4 invProjection;


uniform float time;

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
    vec3 lighting = texture(LightTexture, uv).xyz;
    vec3 normal = normalize(texture(GNorm,uv).xyz);
    vec3 pos = reconstructWorldPos(uv);


    vec3 indirect = vec3(0.0);
    int numSamples = 500;

    for (int i = 0; i < numSamples; i++)
    {
        vec2 rand = vec2(fract(sin(dot(uv, vec2(12.9898,78.233))) * 43758.5453 + float(i)));
        float angle = rand.x * 6.2831 + time * 0.37;
        float radius = rand.y * 0.1;
        vec2 offsetUV = uv + radius * vec2(cos(angle), sin(angle));

        vec3 neighborPos = reconstructWorldPos(offsetUV);
        vec3 nieghborCol = texture(LightTexture, offsetUV).rgb;

        if (dot(neighborPos - pos, normal) > 0.0)
        {
            indirect += nieghborCol;
        }
    }

    indirect /= float(numSamples);
    lighting += indirect;
    SSGITexture = lighting;
}