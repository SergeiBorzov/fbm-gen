#include "../Input/Input.h"

#include "Camera.h"

namespace fbmgen {

    void Camera::Start() {
        m_CurrentCursorX = Input::GetCursorX();
        m_CurrentCursorY = Input::GetCursorY();
        m_LastCursorX = m_CurrentCursorX;
        m_LastCursorY = m_CurrentCursorY;
    }

    void Camera::Update(float dt) {
        f64 deltaX = (m_CurrentCursorX - m_LastCursorX) * sensitivity;
        f64 deltaY = (m_CurrentCursorY - m_LastCursorY) * sensitivity;

        m_LastCursorX = m_CurrentCursorX;
        m_LastCursorY = m_CurrentCursorY;

        m_Yaw += deltaX;
        m_Pitch += deltaY;

        if (m_Pitch > 89.0f) {
            m_Pitch = 89.0f;
        }

        if (m_Pitch < -89.0f) {
            m_Pitch = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, {0.0f, 1.0f, 0.0f}));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));

        if (Input::GetKey(KeyCode::A)) {
            position -= m_Right*speed*dt;
        }
        else if (Input::GetKey(KeyCode::D)) {
            position += m_Right*speed*dt;
        }

        if (Input::GetKey(KeyCode::W)) {
            position += m_Front*speed*dt;
        }
        else if (Input::GetKey(KeyCode::S)) {
            position -= m_Front*speed*dt;
        }

        if (Input::GetKey(KeyCode::Space)) {
            position += m_Up*speed*dt;
        }
        else if (Input::GetKey(KeyCode::LeftShift)) {
            position -= m_Up*speed*dt;
        }
    }

}