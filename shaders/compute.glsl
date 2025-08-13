#version 430

layout(local_size_x = 16, local_size_y = 16) in;

struct SDFPrimitive 
{
    int type;        // 4 bytes
    vec3 pos;        // 12 bytes (16 byte aligned)
    vec3 size;       // 12 bytes (16 byte aligned)
    vec3 rotation;   // 12 bytes (16 byte aligned)
    float radius;    // 4 bytes
    float pad1;      // padding
    float pad2;      // padding
    float pad3;      // padding
};

struct Probe
{
    vec3 pos;
    float pad1;
    vec3 col;
    float pad2;
};

struct Light 
{
    vec3 pos;
    float pad0;
    vec3 col;
    float pad1;
    float Linear;
    float Quadratic;
    float Cutoff;
    float pad2;
    vec3 Dir;
    float pad3; 
};


layout(std430, binding = 0 ) buffer SDFBuffer 
{
    SDFPrimitive primitives[];
};

layout(std430, binding = 1) buffer ProbeBuffer
{
    Probe probes[];
};

layout(std430, binding = 2) buffer LightBuffer
{
    Light lights[];
};

layout(std430, binding = 3) buffer IndirectResultBuffer
{
    vec3 indirectLight[];
};

uniform int numProbes;
uniform int numSDFs;
uniform int numLights;
uniform ivec2 Resolution;
float surfaceEps = 0.001;
float maxDistance = 50.0;
int raysPerProbe;
int maxSteps = 128;

float sdSphere( vec3 p, float s) 
{
    return length(p)-s;
}

float sdBox( vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdScene(vec3 p)
{
    float dist = 1e9;
    for (int i =0; i < primitives.length(); i++)
    {
        vec3 localP = p - primitives[i].pos;
        if (primitives[i].type == 0) // Box;
        {
            dist = min(dist, sdBox(localP,primitives[i].size));
        }
        else if (primitives[i].type == 1) // Sphere
        {
            dist = min(dist, sdSphere(localP, primitives[i].radius));
        }
    }
    return dist;
}

bool isBlocked(vec3 start, vec3 dir, float maxDist)
{
    float travel = 0.0;
    vec3 pos = start;

    for (int i = 0; i < maxSteps; i++)
    {
        float h = sdScene(pos);
        if (h < surfaceEps) return false; // blocked
        travel += h;
        if (travel >= maxDist) break; // Light hit
        if (travel >= maxDistance) break; // higher than global cull
        pos += dir * h;
    }
    return true;
}

vec3 directLighting(vec3 hitPos, vec3 normal, Light light) 
{
    vec3 L = light.pos - hitPos;
    float dist = length(L);
    if (dist <= 1e-5) return vec3(0.0);
    vec3 Ldir = normalize(L);

    float cosTheta = dot(Ldir, normalize(-light.Dir));
    if (cosTheta < light.Cutoff) return vec3(0.0);

    vec3 origin = hitPos + normal * (surfaceEps * 3.0);
    if (!isBlocked(origin, Ldir, dist)) return vec3(0.0);

    float NdotL = max(dot(normal, Ldir), 0.0);
    float attenuation = 1.0 / (1.0 + light.Linear * dist + light.Quadratic * dist * dist);

    return light.col * (NdotL * attenuation);
}

vec3 computeIndirectWProbe(Probe probe) 
{
    const int NUM_RAYS = 32;
    vec3 totalLight = vec3(0.0);

    vec3 normal = vec3(0.0,1.0,0.0);
    for (int r = 0; r < NUM_RAYS; r++) 
    {
        float phi = (6.2831853 / NUM_RAYS) * float(r);
        vec3 dir = normalize(vec3(cos(phi), 0.5, sin(phi)));

        vec3 pos = probe.pos;
        float travel = 0.0;
        bool hit = false;
        vec3 hitPos, hitNormal;

        for (int i = 0; i < maxSteps; i++)
        {
            float h = sdScene(pos);
            if(h < surfaceEps) 
            {
                hit = true;
                hitPos = pos;
                float eps = 0.001;
                hitNormal = normalize(vec3(
                    sdScene(pos + vec3(eps, 0, 0)) - sdScene(pos - vec3(eps, 0, 0)),
                    sdScene(pos + vec3(0, eps, 0)) - sdScene(pos - vec3(0, eps, 0)),
                    sdScene(pos + vec3(0, 0, eps)) - sdScene(pos - vec3(0, 0, eps))
                ));
                break;
            }
            travel += h;
            if (travel > maxDistance) break;
            pos += dir * h;
        }
        if (hit) 
        {
            for (int li = 0; li < numLights; li++)
            {
                totalLight += directLighting(hitPos,hitNormal,lights[li]);
            }
        }
    }

    return totalLight / float(NUM_RAYS);
}


void main()
{
    uint probeIndex = gl_GlobalInvocationID.x;
    if (probeIndex >= uint(numProbes)) return;

    Probe probe = probes[probeIndex];
    vec3 indirect = computeIndirectWProbe(probe);

    indirectLight[probeIndex] = indirect;
}