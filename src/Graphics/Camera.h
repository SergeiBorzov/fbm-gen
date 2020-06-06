#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Fly {
    class Camera {
    public:
        glm::mat4 GetView() { return m_View;}
        glm::mat4 GetProjection() { return m_Projection; }
    protected:
        Camera(){}

        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};       
    };

    class OrthoCamera: public Camera {
    public:
        OrthoCamera(float left, float right, 
                    float top, float bottom,
                    float near, float far);

        inline void SetPosition(const glm::vec3& position) { m_Position = position; }
        
        inline glm::vec3 GetPosition() const { return m_Position; }
    private:
        float m_Left;
        float m_Right;
        float m_Top;
        float m_Bottom;
        float m_Near;
        float m_Far;
    };

}