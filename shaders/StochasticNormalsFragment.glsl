#version 430 core
in vec2 uv;

layout (location=0)out vec3 SNRMTexture;

uniform sampler2D GNorm;
uniform sampler2D LightTexture;
uniform sampler2D DepthTexture;

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

void main()
{
    vec3 sN = makeStochasticNormal(uv);
    SNRMTexture = vec3(sN * 0.5 + 0.5);
}


// Citation

// Glsl code is adapted from Shubham Sachdevas
// "Dynamic, Noise Free, Screen Space Diffuse Global Illumination"
// available at https://gamehacker1999.github.io/posts/SSGI/