#pragma once

#include "../Core/types.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fbmgen {

    class Camera {
    public:
        inline glm::vec3 GetRight() const { return m_Right; }
        inline glm::vec3 GetUp() const { return m_Up; }
        inline glm::vec3 GetFront() const { return m_Front; }
        glm::vec3 position = {0.0f, 0.0f, 3.0f};
        f32 speed = 5.0f;
        f32 sensitivity = 2.0f;
        
        void Start();
        void Update(float dt);
    private:
        glm::vec3 m_Right = {0.0f, 0.0f, 1.0f};
        glm::vec3 m_Up = {0.0f, 1.0f, 0.0f};
        glm::vec3 m_Front = {0.0f, 0.0f, -1.0f};
        
        f32 m_Yaw = -90.0f;
        f32 m_Pitch = 0.0f;

        bool m_Init = false;

        f64 m_CurrentCursorX;
        f64 m_CurrentCursorY;
        f64 m_LastCursorX;
        f64 m_LastCursorY;
    };

}