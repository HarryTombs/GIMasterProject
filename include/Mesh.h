#ifndef MESH_H_
#define MESH_H_
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include <QOpenGLWindow>
#include <memory>

class Mesh
{
    std::string m_OBJFilename;
    std::string m_TEXFilename;
    ngl::Vec3 m_POS;
    std::unique_ptr<ngl::Obj> m_MESH;
public:
    Mesh() : m_OBJFilename("models/test2.obj"), m_TEXFilename("textures/wall.jpg"){}  // model by me texture from LearnOpenGL.com
    Mesh(const std::string &_OBJname, const std::string &_TEXname)
        : m_OBJFilename(_OBJname), m_TEXFilename(_TEXname) {} // but available to change if nessisary
    void CreateVAO();
    void Draw();
    void Transform(float _xDif, float _yDif, float _zDif);
    ngl::Transformation m_TRANS;
    unsigned int texId; // I couldn't get the albedo to load properly so used this to confirm the load of the texture
};


#endif