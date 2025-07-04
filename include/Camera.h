#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera 
{
public:
    glm::vec3 CamPos = glm::vec3(0.0f,0.0f,-5.0f);
    glm::vec3 CamTo = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 CamUp = glm::vec3(0.0f,1.0f,0.0f);
    glm::vec3 WorldUp;
    glm::vec3 CamFront;
    glm::vec3 CamRight;
    glm::mat4 ViewMat = glm::mat4(1.0f);
    glm::mat4 ProjecMat = glm::mat4(1.0f);
    float m_FOV = 45.0f;
    float m_aspect = 1.2f;
    float m_near = 0.01f;
    float m_far = 1000.0f; 
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_speed = 0.75f;
    float m_sensitivity = 0.1f;
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