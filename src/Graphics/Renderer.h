#pragma once

#include <cstdint>

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/opencl.h>

#include "../Core/types.h"
#include "ImageExtension.h"
#include "Texture.h"
#include "Camera.h"

namespace fbmgen {

    class Application;

    struct CameraInfo {
        cl_float3 position;
        cl_float3 right;
        cl_float3 up;
        cl_float3 front;
    };

    struct  Parameters {
        cl_float3 sun_direction;
        cl_float3 resolution;
    };

    class Renderer {
    public:
        inline const Texture* GetTexture() const { return m_Texture; }
        inline const char* GetPlatformName() const { return platform_name; }
        inline const char* GetDeviceName() const { return device_name; }

        bool Create(Application* app);
        void Draw();

        inline void SetCamera(Camera* camera) { m_Camera = camera; }

        void RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality = 100);

        inline void SetSunDirection(const glm::vec3& dir) { m_SunDirection = dir; }

        ~Renderer();
    private:
        Application* m_App = nullptr;
        Texture* m_Texture = nullptr;
        Camera* m_Camera = nullptr;

        /* Parameters stuff */
        glm::vec3 m_SunDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));

        /* End of Parameters stuff */
        
        /* OpenCL stuff */
        char* platform_name = NULL;
        char* device_name = NULL;

        cl_platform_id platform_id = 0;
        cl_device_id device_id = 0;
        cl_context context = 0;
        cl_program warp_program = 0;
        cl_kernel kernel = 0;
        cl_command_queue command_queue = 0;
        cl_mem image_object = 0;
        cl_mem camera_buffer = 0;
        /* End of OpenCL stuff */
    };
}