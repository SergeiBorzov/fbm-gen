#pragma once

#include <cstdint>

#include "Texture.h"

namespace fbmgen {

    
    class Renderer {
    public:
        bool Create();
        void Draw();
    private:
        Texture m_Texture;
    };
}