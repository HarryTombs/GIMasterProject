#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <string.h>

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
    float cutoff = 0.95;
};

// SDFs to inherits properties from all given meshes in a scene padding used for SSBO compatibility
struct SDFPrim
{
    int type;           
    float pad0;         
    float pad1;         
    float pad2;         
    glm::vec3 pos;      
    glm::vec3 size;     
    glm::vec3 rotation; 
    float radius;       
    float pad3;         
    float pad4;         
        
};

// Light probes laid throughout scene to gather luminence for DDGI/SDFGI
struct Probe 
{
    glm::vec3 Pos;
    float pad1;
    glm::vec3 Col; 
    float pad2;
};

class Scene
{

    public: 
    std::vector<Model> Meshes;
    std::vector<Camera> Cameras;
    std::vector<SpotLight> Lights;
    std::vector<SDFPrim> sdfprims;

    // current cam allows for active switching between cameras when live
    Camera currentCam;

    // scene information
    std::vector<glm::vec3> camPos = {glm::vec3(0.0f,0.0f,5.0f)};

    // light Dir used for spotlight and enviroment light
    std::vector<glm::vec3> lightPos = {glm::vec3(-2.0f,0.5f,0.0), glm::vec3(3.0f,0.7f,4.0), glm::vec3(0.0f,1.0f,-4.0)};
    std::vector<glm::vec3> lightCol = {glm::vec3(1.0f,1.0f,1.0), glm::vec3(1.0f,1.0f,1.0), glm::vec3(1.0f,0.0f,1.0f)};
    std::vector<glm::vec3> lightDir = {glm::vec3(-1.0f,0.0f,0.0f),glm::vec3(0.5f,0.0f,1.0), glm::vec3(-0.5f,0.0f,-1.0f)};

    // cubes available by default 
    std::vector<glm::vec3> cubePos = {glm::vec3(0.0f,-1.0f,0.0), glm::vec3(0.0f,3.0f,0.0), glm::vec3(0.0f,1.0f,-5.05f), glm::vec3(5.05f,1.0f,0.0f), glm::vec3(-5.05f,1.0f,0.0f), glm::vec3(2.0,0.0,2.0)};
    std::vector<glm::vec3> cubeSca = {glm::vec3(5.0f,0.1f,5.0f), glm::vec3(5.0f,0.1f,5.0f), glm::vec3(5.0f,2.5f,0.1f), glm::vec3(0.1f,2.5f,5.0f), glm::vec3(0.1f,2.5f,5.0f), glm::vec3(0.1,1.0,3.0)};
    
    // custom is describing any non primitive model, taking position and model path
    std::vector<glm::vec3> customPos = {glm::vec3(0.0f,0.0f,0.0)} ;
    std::vector<std::string> customPath = {"models/test2.obj"};
    
    // setup for probes
    std::vector<Probe> probes;

    float RoomMinX = -5.5f;
    float RoomMinY = -1.5f;
    float RoomMinZ = -5.5f;
    float RoomMaxX = 5.5f;
    float RoomMaxY = 3.5f;
    float RoomMaxZ = 5.5f;

    float spacing = 1.0f;

    void layoutProbes();
    void init();


    private:
};

#endif