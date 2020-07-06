#include "Renderer.h"

#include "../Core/fbmgen_gl.h"

namespace fbmgen {

    bool Renderer::Create() {
        m_Texture = Texture::Load("resources/texture/rock.jpg");
        return m_Texture;
    }

    void Renderer::Draw() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT);)
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f);)
    }

    Renderer::~Renderer() {
        if (m_Texture) {
            delete m_Texture;
        }
    }

}