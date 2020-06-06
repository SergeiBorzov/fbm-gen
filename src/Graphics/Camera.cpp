#include "Camera.h"

namespace Fly {
    OrthoCamera::OrthoCamera(float left, float right, 
                             float top, float bottom,
                             float near, float far):
    m_Left(left), m_Right(right),
    m_Top(top), m_Bottom(bottom),
    m_Near(near), m_Far(far)
    {
        m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
    }
}