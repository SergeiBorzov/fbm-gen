#pragma once

#include "../Core/types.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fbmgen {

    class Camera {
    public:
        glm::vec3 position = {0.0f, 0.0f, 3.0f};
        f32 speed = 5.0f;
        f32 sensitivity = 0.1f;

        inline glm::mat4 GetViewMatrix() const { 
            return glm::mat4(glm::vec4(m_Right, 0.0f), 
                             glm::vec4(m_Up, 0.0f), 
                             glm::vec4(m_Forward, 0.0f), 
                             glm::vec4(position, 1.0f)); 
        }

        inline glm::vec3 GetRight() const { return m_Right; }
        inline glm::vec3 GetUp() const { return m_Up; }
        inline glm::vec3 GetFront() const { return m_Forward; }
        
        inline void Reset() { m_Init = false; }
        void Update(float dt);
    private:
        void Start();

        glm::vec3 m_Right = {0.0f, 0.0f, 1.0f};
        glm::vec3 m_Up = {0.0f, 1.0f, 0.0f};
        glm::vec3 m_Forward = {0.0f, 0.0f, -1.0f};
        
        f32 m_Yaw = -90.0f;
        f32 m_Pitch = 0.0f;

        bool m_Init = false;

        f64 m_CurrentCursorX;
        f64 m_CurrentCursorY;
        f64 m_LastCursorX;
        f64 m_LastCursorY;
    };

}