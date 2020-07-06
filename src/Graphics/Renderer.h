#pragma once

#include <cstdint>

#include "../Core/types.h"
#include "Texture.h"

namespace fbmgen {

    class Renderer {
    public:
        bool Create();
        void Draw();

        const Texture* GetTexture() const { return m_Texture; }
        ~Renderer();
    private:
        Texture* m_Texture = nullptr;
    };
}