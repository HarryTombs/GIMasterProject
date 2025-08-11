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

class Scene
{
    std::vector<Model> Meshes;
    std::vector<Camera> Cameras;
    std::vector<BaseLight> Lights;
    Camera currentCam;
};

struct SDFPrim
{
    int type;          // 4 bytes
    glm::vec3 pos;     // 12 bytes
    glm::vec3 size;    // 12 bytes
    glm::vec3 rotation;// 12 bytes
    float radius;      // 4 bytes
    float pad1;        // 4 bytes
    float pad2;        // 4 bytes
    float pad3;        // 4 bytes
};


#endif