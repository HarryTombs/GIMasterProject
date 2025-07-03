#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera 
{
public:
    glm::vec3 CamPos;
    glm::vec3 CamTo;
    glm::vec3 CamUp;
    glm::vec3 WorldUp;
    glm::vec3 CamFront;
    glm::vec3 CamRight;
    glm::mat4 ViewMat;
    glm::mat4 ProjecMat;
    float m_FOV;
    float m_aspect;
    float m_near;
    float m_far; 
    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_sensitivity;
    int width;
    int height;


    Camera()=default;
    Camera(glm::vec3 from,glm::vec3 to,glm::vec3 up);
    void set(glm::vec3 from,glm::vec3 to,glm::vec3 up);
    void setProjection(float FOV,float aspect,float nearPlane,float farPlane);
    void setView();
    void setVectors();
    void processMouseMovement(float newX,float newY,bool contrainPitch = true);
    glm::mat4 getView(){return ViewMat;}
    glm::mat4 getProjection(){return ProjecMat;}

    void Move(float x, float y, float deltaTime);

private:


};
#endif