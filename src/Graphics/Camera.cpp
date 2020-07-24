#include "../Input/Input.h"

#include "Camera.h"

namespace fbmgen {

    void Camera::Start() {
        m_LastCursorX = Input::GetCursorX();
        m_LastCursorY = Input::GetCursorY();
    }

    void Camera::Update(float dt) {
        if (!m_Init) {
            Start();
            m_Init = true;
        }
        m_CurrentCursorX = Input::GetCursorX();
        m_CurrentCursorY = Input::GetCursorY();
        f64 deltaX = (m_CurrentCursorX - m_LastCursorX) * sensitivity;
        f64 deltaY = (m_LastCursorY - m_CurrentCursorY) * sensitivity;

        m_LastCursorX = m_CurrentCursorX;
        m_LastCursorY = m_CurrentCursorY;

        m_Yaw += (f32)deltaX;
        m_Pitch += (f32)deltaY;

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
        m_Forward = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Forward, {0.0f, 1.0f, 0.0f}));
        m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

        //printf("m_Right: %f, %f, %f\n", m_Right.x, m_Right.y, m_Right.z);
        if (Input::GetKey(KeyCode::A)) {
            position -= m_Right*speed*dt;
        }
        else if (Input::GetKey(KeyCode::D)) {
            position += m_Right*speed*dt;
        }

        if (Input::GetKey(KeyCode::W)) {
            position += m_Forward*speed*dt;
        }
        else if (Input::GetKey(KeyCode::S)) {
            position -= m_Forward*speed*dt;
        }

        if (Input::GetKey(KeyCode::Space)) {
            position += m_Up*speed*dt;
        }
        else if (Input::GetKey(KeyCode::LeftShift)) {
            position -= m_Up*speed*dt;
        }
    }

}