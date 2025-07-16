#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Camera.h"

class Scene
{
    std::vector<Mesh> Meshes;
    std::vector<Camera> Cameras;

    Camera currentCam;
};

#endif