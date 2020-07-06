#pragma once

#include <cstdint>

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/opencl.h>

#include "../Core/types.h"
#include "Texture.h"

namespace fbmgen {
    class Renderer {
    public:
        inline const Texture* GetTexture() const { return m_Texture; }
        inline const char* GetPlatformName() const { return platform_name; }
        inline const char* GetDeviceName() const { return device_name; }

        bool Create();
        void Draw();

        ~Renderer();
    private:

        Texture* m_Texture = NULL;


        
        char* platform_name = NULL;
        char* device_name = NULL;

        
        cl_platform_id platform_id = 0;
        cl_device_id device_id = 0;
        cl_context context = 0;
        cl_program warp_program = 0;
       
    };
}