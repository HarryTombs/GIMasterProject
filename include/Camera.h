#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW = -90.f; 
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera 
{
public:
    glm::vec3 CamPos;
    glm::vec3 CamUp;
    glm::vec3 WorldUp;
    glm::vec3 CamFront;
    glm::vec3 CamRight;

    float m_zoom;
    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_sensitivity;
    int width;
    int height;

    Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f),glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f),float yaw = YAW, float pitch = PITCH) : CamFront(glm::vec3(0.0f,0.0f,-1.0f)), m_speed(SPEED), m_sensitivity(SENSITIVITY), m_zoom(ZOOM)    {
        CamPos = position;
        WorldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        setVectors();
    }
    void processMouseMovement(float newX,float newY,bool contrainPitch = true);
    glm::mat4 getView(){return glm::lookAt(CamPos, CamPos + CamFront, CamUp);};
    void Move(float x, float y, float deltaTime);

private:
    void setVectors()
    {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(m_yaw) * cos(glm::radians(m_pitch)));
        newFront.y = sin(glm::radians(m_pitch));
        newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        CamFront = glm::normalize(newFront);
        
        CamRight = glm::normalize(glm::cross(CamFront,WorldUp));
        CamUp = glm::normalize(glm::cross(CamRight,CamFront));
        std::cout << "RIGHT: " << glm::to_string(CamRight) << std::endl;
        std::cout << "UP: " << glm::to_string(CamUp) << std::endl;
        std::cout << "FRONT: " << glm::to_string(CamFront) << std::endl;
    };


};
#endif