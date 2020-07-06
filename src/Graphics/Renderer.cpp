#include "Renderer.h"
#include "../Core/fbmgen_gl.h"

namespace fbmgen {

    bool Renderer::Create() {
        // Glew initialization
        glewExperimental = true; 
        if (glewInit() != GLEW_OK) { 
            return false;
        }


        // OpenCL
        
        /* Platform stuff begin*/
        cl_platform_id platform_id;
        
        if (clGetPlatformIDs(1, &platform_id, NULL) < 0 ) {
            fprintf(stderr, "Error getting platform!\n");
            return false;
        }

        size_t platform_name_size;
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) < 0) {
            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }

        platform_name = static_cast<char*>(malloc(platform_name_size));
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL) < 0 ) {
            free(platform_name);
            platform_name = NULL;

            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }
        /* Platform stuff end */

        /* Device stuff begin */
        cl_device_id device_id;
        if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL) < 0 ) {
            free(platform_name);
            platform_name = NULL;

            fprintf(stderr, "Error getting device!\n");
            return false;
        }

        size_t device_name_size;
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) < 0) {
            free(platform_name);
            platform_name = NULL;

            fprintf(stderr, "Error getting device info!\n");
            return false;
        }

        device_name = static_cast<char*>(malloc(device_name_size));
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) < 0) {
            free(device_name);
            device_name = NULL;
            free(platform_name);
            platform_name = NULL;
            fprintf(stderr, "Error getting device info!\n");
        }
        /* Device stuff end */


        /* OpenCL context begin */

        
        /* OpenCL context end */

        //context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);



        m_Texture = Texture::Load("resources/texture/rock.jpg");
        return m_Texture;
    }

    void Renderer::Draw() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT);)
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f);)
    }

    Renderer::~Renderer() {

        if (device_name) {
            free(device_name);
        }

        if (platform_name) {
            free(platform_name);
        }

        if (m_Texture) {
            delete m_Texture;
        }
    }

}