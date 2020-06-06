#pragma once

#include <cstdint>

namespace fbmgen {

    class Texture {
    public:
        bool Load(const char* filename);
        ~Texture();
    private:
        uint32_t m_Handle;
        bool m_Inited = false;
    };

}