#version 430 core
in vec2 uv;

layout (location=0)out vec3 SSGITexture;

uniform sampler2D GNorm;
uniform sampler2D LightTexture;
uniform sampler2D DepthTexture;
uniform sampler2D SNRMTexture;

uniform mat4 invView;
uniform mat4 invProjection;
uniform mat4 projection;
uniform mat4 view;

uniform int frame;
uniform vec2 Resolution;

int StepCount   = 32;
float Thickness = 0.2;
float MaxDist   = 10.0;
float StepScale = 1.0;

float has11(float x)
{
    x = fract(x * 0.1031);
    x *= x + 33.33;
    x *= x + x;
    return fract(x);
}

float hash21(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float IGN(vec2 pix, int frame) 
{
    float f = float(frame);
    return hash21(pix + vec2(f, f*0.618));
}

vec2 rng2(vec2 pix, int frame)
{
    float r1 = IGN(pix, frame);
    float r2 = IGN(pix + 0.37, frame * 13 + 7);
    return vec2(r1, r2);
}

bool projectViewToUV(vec3 viewPos, out vec2 uv, out float ndcZ)
{
    vec4 clip = projection * vec4(viewPos, 1.0);
    if(clip.w <= 0.0) return false;
    vec3 ndc = clip.xyz / clip.w;
    uv = ndc.xy * 0.5 + 0.5;
    ndcZ = ndc.z;
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

vec3 ndcToView(vec3 ndc)
{
    vec4 v = invProjection * vec4(ndc, 1.0);
    return v.xyz / v.w;
}

vec3 reconstructViewPos(vec2 uv, float depth)
{
    float z = depth * 2.0 - 1.0;
    vec3 ndc = vec3(uv * 2.0 - 1.0, z);
    return ndcToView(ndc);
}

vec4 RayMarchSSGI(vec3 originVS, vec3 dirVS)
{
    dirVS = normalize(dirVS);
    vec2 pix = uv * Resolution;
    vec2 jitter = rng2(pix, frame);
    jitter = jitter * 0.5 + 0.25;
    float stepSize = (MaxDist / float(max(StepCount,1))) * StepScale;
    stepSize *= jitter.x = jitter.y;
    float t = stepSize;
    vec2 bestUV = uv;
    float bestT = 0.0;

    for (int i = 0; i < StepCount; i++)
    {
        vec3 pVS = originVS + dirVS * t;
        if (length(pVS - originVS) > MaxDist) break;
        vec2 uv; 
        float ndcZ;
        if (!projectViewToUV(pVS, uv, ndcZ)) break;
        float sceneDepth = texture(DepthTexture,uv).r;
        vec3 sceneVS = reconstructViewPos(uv, sceneDepth);
        float dz = pVS.z - sceneVS.z;
        bool depthInFront = pVS.z >= sceneVS.z - Thickness;
        bool ndcValid = ndcZ <= 1.0;

        if (ndcValid && abs(dz) <= Thickness && depthInFront)
        {
            float tBack = t - stepSize;
            float tFront = t;
            for (int j = 0; j < 5; j++) 
            {
                float tm = 0.5 * (tBack + tFront);
                vec3 mVS = originVS + dirVS * tm;
                vec2 mUV; float mZ;
                if (!projectViewToUV(mVS, mUV, mZ)) break;
                float md = texture(DepthTexture,mUV).r;
                vec3 mSceneVS = reconstructViewPos(mUV, md);
                float mdz = mVS.z - mSceneVS.z;
                if (abs(mdz) <= Thickness) 
                {
                    tFront = tm;
                    bestUV = mUV;
                    bestT = tm;
                }
                else 
                {
                    tBack = tm;
                }
            }
            retun vec4(bestUV, bestT, 1.0);
        }
        t += stepSize;
    }
    return vec4(bestUV,0.0,0.0);
}

vec3 fetchStochasticViewDir(vec2 uv)
{
    vec3 sN = texture(SNRMTexture, uv).xyz;
    return normalize(sN * 2.0 -1.0);
}


void main() 
{
    float depth = texture(DepthTexture,uv).r;
    vec3 viewPos = reconstructViewPos(uv, depth);
    vec3 viewDir = fetchStochasticViewDir(uv);
    viewDir = normalize(mat3(view) * viewDir);
    vec4 hit = RayMarchSSGI(viewPos, viewDir);
    vec2 hitUV = hit.xy;
    float hitMask = hit.z;
    vec3 cubemap = vec3(0.0);
    vec3 sampleColour = texture(LightTexture, hitUV).rgb;
    // sampleColour *= hitMask;
    SSGITexture =  sampleColour;
}