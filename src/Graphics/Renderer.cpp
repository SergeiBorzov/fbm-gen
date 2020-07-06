#include <windows.h>

#include "../Core/fbmgen_gl.h"
#include "../Core/utils.h"
#include "Renderer.h"

namespace fbmgen {

    bool Renderer::Create() {
        // GLEW:
        glewExperimental = true; 
        if (glewInit() != GLEW_OK) { 
            return false;
        }

        // OpenCL:        
        /* Platform stuff begin*/
        if (clGetPlatformIDs(1, &platform_id, NULL) < 0 ) {
            fprintf(stderr, "Error getting platform!\n");
            return false;
        }

        size_t platform_name_size;
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) < 0) {
            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }

        platform_name = new char[platform_name_size];
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL) < 0 ) {
            delete platform_name;
            platform_name = nullptr;

            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }
        /* Platform stuff end */

        /* Device stuff begin */
        if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL) < 0 ) {
            delete platform_name;
            platform_name = NULL;

            fprintf(stderr, "Error getting device!\n");
            return false;
        }

        size_t device_name_size;
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) < 0) {
            delete platform_name;
            platform_name = NULL;

            fprintf(stderr, "Error getting device info!\n");
            return false;
        }

        device_name = new char[device_name_size];
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) < 0) {
            delete device_name;
            device_name = NULL;
            delete platform_name;
            platform_name = NULL;
            fprintf(stderr, "Error getting device info!\n");
        }
        /* Device stuff end */


        /* OpenCL context begin */
        cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id,
            0
        };

        context = clCreateContext(properties, 1, &device_id, NULL, NULL, NULL);
        if (!context) {
            fprintf(stderr, "Failed to create OpenCL Context!\n");
            return false;
        }
        /* OpenCL context end */

        /* Create OpenGL texture and share it - begin */
        m_Texture = Texture::CreateEmpty(1024, 620, 4);
        if (!clCreateFromGLTexture(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, m_Texture->GetHandle(), NULL)) {
            fprintf(stderr, "Failed to create shared texture between OpenGL and OpenCL\n");
            return false;
        };
        /* Create OpenGL texture and share it - end */


        /* OpenCL Program - begin */
        char* source_string;
        size_t source_string_size = 0;
        ReadFileToString("resources/kernel/warp.cl", &source_string, &source_string_size);
        size_t source_string_length = source_string_size / sizeof(char);


        const char* source = source_string;
        warp_program = clCreateProgramWithSource(context, 1, &source, &source_string_length, NULL);
        if (!warp_program) {
            fprintf(stderr, "Failed to create OpenCL program!\n");
            return false;
        }
        /* OpenCL Program - end */

        
        
        return m_Texture;
    }

    void Renderer::Draw() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT);)
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f);)

        //glFinish()
        //clEnqueueAcquireGLObjects();

        //clEnqueueNDRangeKernel();
        //clEnqueueReleaseGLObjects();
        //clFinish();
    }

    Renderer::~Renderer() {
        if (warp_program) {
            clReleaseProgram(warp_program);
        }

        if (m_Texture) {
            delete m_Texture;
        }

        if (context) {
            clReleaseContext(context);
        }


        if (device_name) {
            delete device_name;
        }

        if (platform_name) {
            delete platform_name;
        }
    }

}