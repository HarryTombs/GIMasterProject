#version 430 core
in vec2 uv;

layout (location=0)out vec3 BlurTexture;

uniform sampler2D GNorm;
uniform sampler2D SSGITexture;
uniform sampler2D DepthTexture;

uniform vec2 texelSize;

void main() 
{
    float currentDepth = texture(DepthTexture, uv).r;
    vec3 currentNorm = normalize(texture(GNorm,uv).xyz);
    vec3 result = vec3(0.0);

    float totalWeight = 0.0;

    for (int x = -2; x<=2; x++ )
    {
        for (int y = -2; y<=2; y++)
        {
            vec2 offset = vec2(x,y) * texelSize;
            vec2 sampleUV = uv + offset;

            float sampleDepth = texture(DepthTexture, sampleUV).r;
            vec3 sampleNorm = normalize(texture(GNorm, sampleUV)).xyz;
            vec3 sampleColour = texture(SSGITexture, sampleUV).xyz;

            float depthDiff = abs (currentDepth - sampleDepth);
            float wDepth = exp(-depthDiff * 80.0);

            float ndot = max(dot(currentNorm, sampleNorm),0.0);
            float wNormal = pow(ndot, 8.0);

            float wSpatial = 1.0 - (abs(x) + abs(y)) * 0.25;

            float weight = wDepth * wNormal * wSpatial;
            result += sampleColour * weight;
            totalWeight += weight;
        }
    }

    vec3 SSGI = texture(SSGITexture,uv).xyz;
    BlurTexture = result / totalWeight;
}