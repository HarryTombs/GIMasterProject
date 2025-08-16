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

void Scene::init()
{
    layoutProbes();
    for(int i = 0; i < cubeSca.size(); i++)
    {
        Model newCube("",true);
        newCube.translate(cubePos[i]);
        newCube.scale(cubeSca[i]);
        newCube.type = 0;
        Meshes.push_back(newCube);
    }
    for(int i = 0; i < customPos.size(); i++)
    {
        Model newModel(customPath[i]);
        newModel.translate(customPos[i]);
        newModel.type = 1;
        Meshes.push_back(newModel);
    }
    for(Model m : Meshes)
    {
        SDFPrim newSdf;
        newSdf.pos = m.pos;
        if (m.type == 0)
        {
            newSdf.size = m.sca;
        }
        if (m.type == 1)
        {
            newSdf.radius = 1.0f;
        }
        newSdf.type = m.type;
        sdfprims.push_back(newSdf);
    }
    for (int i = 0; i < lightPos.size(); i++)
    {
        SpotLight newLight; 
        newLight.pos = lightPos[i];
        newLight.col = lightCol[i];
        newLight.direction = lightDir[i];
        Lights.push_back(newLight);
    }
    for (int i = 0; i < camPos.size(); i++)
    {
        Camera newCam(camPos[i]);
        Cameras.push_back(newCam);
    }
    currentCam = Cameras[0];

}