#include "Scene.h"

void Scene::layoutProbes()
{
    for(float x = RoomMinX; x <= RoomMaxX; x + spacing)
    {
        for(float y = RoomMinX; y <= RoomMaxX; y + spacing)
        {
            for(float z = RoomMinX; z <= RoomMaxX; z + spacing)
            {
                Probe p;
                p.Pos = glm::vec3(x,y,z);
                p.pad = 0.0f;
                probes.push_back(p);
            }
        }
    }

}