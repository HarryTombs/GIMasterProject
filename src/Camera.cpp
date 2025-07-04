#include "Camera.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Camera::Camera(glm::vec3 from,glm::vec3 to,glm::vec3 up)
{
    CamPos = from;
    CamTo = to;
    CamUp = up;

    setVectors();
}

void Camera::set(glm::vec3 from,glm::vec3 to,glm::vec3 up)
{
    CamPos = from;
    CamTo = to;
    CamUp = up;
    setVectors();
}

void Camera::setProjection(float FOV,float aspect,float nearPlane,float farPlane)
{
    m_FOV = FOV;
    m_aspect = aspect;
    m_near = nearPlane;
    m_far = farPlane;
    ProjecMat = glm::perspective(glm::radians(FOV), aspect, nearPlane, farPlane);
}

void Camera::setVectors()
{
    // CamFront.x = cosf(glm::radians(m_yaw) * cosf(glm::radians(m_pitch)));
    // CamFront.y = sinf(glm::radians(m_pitch));
    // CamFront.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    // CamFront = glm::normalize(CamFront);
    

    // CamRight = glm::cross(CamFront,WorldUp);
    // CamUp = glm::cross(CamRight,CamFront);

    // CamRight = glm::normalize(CamRight);
    // CamFront = glm::normalize(CamFront);

    //FIX ROTATION STUFF LATER

    glm::vec3 lookat = CamTo + CamFront;


    std::cout << "looking at: " << glm::to_string(lookat) << std::endl;
    std::cout << "CamPos: " << glm::to_string(CamPos) << std::endl;
    std::cout << "CamUp: " << glm::to_string(CamUp) << std::endl;
    std::cout << "CamFront: " << glm::to_string(CamFront) << std::endl;

    ViewMat = glm::lookAt(CamPos,lookat, CamUp);

    std::cout <<  "ViewMat " << glm::to_string(ViewMat) << std::endl;
}

void Camera::Move(float x, float y, float deltaTime)
{
    float velocity = m_speed * deltaTime;
    CamPos += CamFront * velocity * x;
    CamPos += CamRight * velocity * y;
    setVectors();
}

void Camera::processMouseMovement(float newX,float newY,bool contrainPitch)
{
    newX *= m_sensitivity;
    newY *= m_sensitivity;

    m_yaw -= newX;
    m_pitch -= newY;

    if (contrainPitch)
    {
        if (m_pitch > 89.0)
            m_pitch = 89.0;
        if (m_pitch < -89.0)
            m_pitch = -89.0;
    }

    setVectors();
}