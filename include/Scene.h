#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Camera.h"
#include <glm/glm.hpp>

enum lightType
{
    POINT,
    DIRECTION,
    SPOT
};

struct BaseLight
{
    glm::vec3 pos;
    glm::vec3 col;
    float quadratic = 0.1f;
    float linear = 0.1f;
    virtual ~BaseLight() {}
};

struct SpotLight : BaseLight
{
    glm::vec3 direction;
    float cutoff = 0.95f;
};


struct SDFPrim
{
    int type;          
    glm::vec3 pos;     
    glm::vec3 size;    
    glm::vec3 rotation;
    float radius;      
    float pad1;        
    float pad2;        
    float pad3;        
};

struct Probe 
{
    glm::vec3 Pos;
    float pad1;
    glm::vec3 Col; // storing indirect light values
    float pad2;
};

class Scene
{

    public: 
    std::vector<Model> Meshes;
    std::vector<Camera> Cameras;
    std::vector<SpotLight> Lights;
    std::vector<SDFPrim> sdfprims;

    Camera currentCam;


    std::vector<Probe> probes;

    float RoomMinX = -5.5f;
    float RoomMinY = -1.5f;
    float RoomMinZ = -5.5f;
    float RoomMaxX = 5.5f;
    float RoomMaxY = 3.5f;
    float RoomMaxZ = 5.5f;

    float spacing = 1.0f;

    void layoutProbes();

    private:
};

#endif