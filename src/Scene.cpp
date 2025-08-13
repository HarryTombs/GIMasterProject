#include "Scene.h"

void Scene::layoutProbes()
{
    for(float x = RoomMinX; x <= RoomMaxX; x += spacing)
    {
        for(float y = RoomMinY; y <= RoomMaxY; y += spacing)
        {
            for(float z = RoomMinZ; z <= RoomMaxZ; z += spacing)
            {
                Probe p;
                p.Pos = glm::vec3(x,y,z);
                p.pad1 = 0.0f;
                p.pad2 = 0.0f;
                probes.push_back(p);
            }
        }
    }
    return;
}