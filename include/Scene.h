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

    // You could put all this in a json huh?

    std::vector<glm::vec3> camPos = {glm::vec3(0.0f,0.0f,5.0f)};

    std::vector<glm::vec3> lightPos = {glm::vec3(-2.0f,0.5f,0.0), glm::vec3(3.0f,0.7f,4.0), glm::vec3(0.0f,1.0f,-4.0)};
    std::vector<glm::vec3> lightCol = {glm::vec3(1.0f,0.0f,0.0), glm::vec3(0.0f,1.0f,0.0), glm::vec3(0.0f,0.0f,1.0f)};
    std::vector<glm::vec3> lightDir = {glm::vec3(-1.0f,0.0f,0.0f),glm::vec3(0.5f,0.0f,1.0), glm::vec3(-0.5f,0.0f,-1.0f)};

    std::vector<glm::vec3> cubePos = {glm::vec3(0.0f,-1.0f,0.0), glm::vec3(0.0f,3.0f,0.0), glm::vec3(0.0f,1.0f,-5.05f), glm::vec3(5.05f,1.0f,0.0f), glm::vec3(-5.05f,1.0f,0.0f)};
    std::vector<glm::vec3> cubeSca = {glm::vec3(5.0f,0.1f,5.0f), glm::vec3(5.0f,0.1f,5.0f), glm::vec3(5.0f,2.5f,0.1f), glm::vec3(0.1f,2.5f,5.0f), glm::vec3(0.1f,2.5f,5.0f)};

    std::vector<glm::vec3> customPos = {glm::vec3(0.0f,0.0f,0.0), glm::vec3(2.0f,0.0f,0.0f)} ;
    std::vector<std::string> customPath = {"models/test2.obj","models/test2.obj"};

    // end of sarcastic inclusion

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