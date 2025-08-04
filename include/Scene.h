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

struct Light
{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec3 direction;
    lightType type;
};

class Scene
{
    std::vector<Model> Meshes;
    std::vector<Camera> Cameras;
    std::vector<Light> Lights;
    Camera currentCam;
};

#endif