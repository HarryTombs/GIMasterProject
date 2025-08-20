#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW = -90.f; 
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.2f;
const float ZOOM = 45.0f;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

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


    Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f),glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f),float yaw = YAW, float pitch = PITCH) : CamFront(glm::vec3(0.0f,0.0f,-1.0f)), m_speed(SPEED), m_sensitivity(SENSITIVITY), m_zoom(ZOOM)    {
        CamPos = position;
        WorldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        setVectors();
    }
    void Move(Camera_Movement direction, float deltaTime)
    {
        float velocity = m_speed * deltaTime;
        if (direction == FORWARD)   CamPos += CamFront * velocity;
        if (direction == BACKWARD)  CamPos -= CamFront * velocity;
        if (direction == LEFT)      CamPos -= CamRight * velocity;
        if (direction == RIGHT)     CamPos += CamRight * velocity;
        if (direction == UP)        CamPos += CamUp * velocity;
        if (direction == DOWN)      CamPos -= CamUp * velocity;

    }
    void processMouseMovement(float newX,float newY,bool contrainPitch = true)
    {
        newX *= m_sensitivity;
        newY *= m_sensitivity;

        m_yaw += newX;
        m_pitch += newY;

        if (contrainPitch = true)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
        }
        setVectors();
    }
    glm::mat4 getView(){return glm::lookAt(CamPos, CamPos + CamFront, CamUp);};


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
    };


};
#endif