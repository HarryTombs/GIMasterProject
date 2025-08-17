#version 430 core
in vec2 uv;

layout (location=0)out vec3 SSGITexture;

uniform sampler2D GNorm;
uniform sampler2D LightTexture;
uniform sampler2D DepthTexture;

uniform mat4 invView;
uniform mat4 invProjection;
uniform mat4 projection;
uniform mat4 view;

uniform int frame;
uniform vec2 Resolution;

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

vec3 getPerpendicularVector(vec3 n)
{
    if(n.z < -0.99999999)
    {
        return vec3(0.0, -1.0, 0.0);
    }
    
    float a = 1.0 / (1.0 + n.z);
    float b = -n.x * n.y * a;
    return vec3(1.0 - n.x * n.x * a, b, -n.x);
}

vec3 cosineSampleHemiSsphere(float u1, float u2, vec3 N)
{
    float r = sqrt(u1);
    float phi = 6.28318530718 * u2; 

    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = sqrt(max(0.0,1.0 - u1));

    vec3 B = normalize(getPerpendicularVector(N));
    vec3 T = normalize(cross(B, N));

    return T * x + B * y + N * z;
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

bool projectViewToUV(vec3 viewPos, out vec2 uv, out float ndcZ)
{
    vec4 clip = projection * vec4(viewPos, 1.0);
    if(clip.w <= 0.0) return false;
    vec3 ndc = clip.xyz / clip.w;
    uv = ndc.xy * 0.5 + 0.5;
    ndcZ = ndc.z;
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

// stochastic Normals

vec3 fetchWorldOrViewNormal(vec2 uv) 
{
    vec3 n = texture(GNorm, uv).xyz;
    n = normalize(n * 2.0 - 1.0);
    return n;
}

vec3 makeStochasticNormal(vec2 uv)
{
    vec3 baseN = fetchWorldOrViewNormal(uv);
    vec2 pix = uv * Resolution;

    vec2 r = rng2(pix, frame);

    vec3 s = cosineSampleHemiSsphere(r.x, r.y, baseN);
    return normalize(s);
}

void fragStochasticNormals()
{
    vec3 sN = makeStochasticNormal(uv);
    // FragColor = vec4(sN * 0.5 + 0.5,1.0);
}


vec3 reconstructWorldPos(vec2 uv) {
    float depth = texture(DepthTexture, uv).r;
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

    vec4 viewPos = invProjection * clip;
    viewPos /= viewPos.w;

    vec4 worldPos = invView * viewPos;
    return worldPos.xyz;
}

vec3 reconstructViewPos(vec2 uv) {
    float depth = texture(DepthTexture, uv).r;
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = invProjection * clip;
    return viewPos.xyz / viewPos.w;
}

// random helper
float rand(vec2 co, float seed) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233)) + seed) * 43758.5453);
}

// hemisphere sample in view-space
vec3 hemisphereSample(vec3 normal, vec2 xi) {
    float phi = 2.0 * 3.14159265 * xi.x;
    float cosTheta = pow(1.0 - xi.y, 0.5);   // cosine-weighted
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // build tangent basis
    vec3 tangent = normalize(abs(normal.z) < 0.999 ? cross(normal, vec3(0,0,1)) : cross(normal, vec3(0,1,0)));
    vec3 bitangent = cross(normal, tangent);

    return normalize(
        tangent * cos(phi) * sinTheta +
        bitangent * sin(phi) * sinTheta +
        normal * cosTheta
    );
}

void main() {
    vec3 lighting = texture(LightTexture, uv).rgb;
    vec3 normalWorld = normalize(texture(GNorm, uv).xyz);
    vec3 posWorld = reconstructWorldPos(uv);

    // transform normal and pos into view space
    vec3 posView = (view * vec4(posWorld, 1.0)).xyz;
    vec3 normalView = normalize((view * vec4(normalWorld, 0.0)).xyz);

    vec3 indirect = vec3(0.0);
    int numSamples = 150;
    float radius = 0.25;
    float depthCentre = -posView.z;
    float bias = 0.1; 

    for (int i = 0; i < numSamples; i++) {
        // stratified random
        vec2 xi = vec2(
            rand(uv * float(i), frame + float(i)),
            rand(uv * float(i+1), frame - float(i))
        );

        vec3 dir = hemisphereSample(normalView, xi);
        vec3 samplePosView = posView + dir * radius;

        // project to screen
        vec4 proj = projection * vec4(samplePosView, 1.0);
        proj /= proj.w;
        vec2 sampleUV = proj.xy * 0.5 + 0.5;

        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            continue;

        float sceneDepth = texture(DepthTexture, sampleUV).r;
        vec3 sceneViewPos = reconstructViewPos(sampleUV);

        if (abs(sceneViewPos.z - samplePosView.z) < bias) {
            vec3 sampleLight = texture(LightTexture, sampleUV).rgb;
            indirect += sampleLight;
        }
    }
    indirect = clamp(indirect, vec3(0.0), vec3(2.6));
    indirect /= float(numSamples);

    SSGITexture =  indirect;
}