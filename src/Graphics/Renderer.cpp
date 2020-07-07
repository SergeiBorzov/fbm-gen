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
        if (clGetPlatformIDs(1, &platform_id, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error getting platform!\n");
            return false;
        }

        size_t platform_name_size;
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, NULL, &platform_name_size) != CL_SUCCESS) {
            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }

        platform_name = new char[platform_name_size];
        if (clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, platform_name_size, platform_name, NULL) != CL_SUCCESS) {
            delete[] platform_name;
            platform_name = nullptr;

            fprintf(stderr, "Error getting platform info!\n");
            return false;
        }
        /* Platform stuff end */

        /* Device stuff begin */
        if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL) != CL_SUCCESS) {
            delete[] platform_name;
            platform_name = NULL;

            fprintf(stderr, "Error getting device!\n");
            return false;
        }

        size_t device_name_size;
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size) != CL_SUCCESS) {
            delete[] platform_name;
            platform_name = NULL;

            fprintf(stderr, "Error getting device info!\n");
            return false;
        }

        device_name = new char[device_name_size];
        if (clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name_size, device_name, NULL) != CL_SUCCESS) {
            delete[] device_name;
            device_name = NULL;
            delete[] platform_name;
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

        /* OpenCL Program - begin */
        char* source_string;
        size_t source_string_size = 0;


        char path_to_program[MAX_PATH];
        GetAbsolutePath(path_to_program, MAX_PATH);
        strncat(path_to_program, "resources/kernel/warp.cl", MAX_PATH - strlen(path_to_program) - 1);

        if (!ReadFileToString(path_to_program, &source_string, &source_string_size)) {
            return false;
        }
        
        size_t source_string_length = source_string_size / sizeof(char);
 
        const char* source = source_string;
        warp_program = clCreateProgramWithSource(context, 1, &source, &source_string_length, NULL);
        if (!warp_program) {
            fprintf(stderr, "Failed to create OpenCL program!\n");
            return false;
        }
       
        const char* options = "-Werror";
        if (clBuildProgram(warp_program, 1, &device_id, options, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Failed to compile kernel!\n");

            size_t log_size = 0;
            clGetProgramBuildInfo(warp_program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            char* program_log = new char[log_size+1];
            program_log[log_size] = '\0';

            clGetProgramBuildInfo(warp_program, device_id, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);

            fprintf(stderr, "Log: %s\n", program_log);
            delete[] program_log;
            return false;
        }

        free(source_string);
        /* OpenCL Program - end */

        /* OpenCL Kernel - begin */
        kernel = clCreateKernel(warp_program, "main", NULL);
        if (!kernel) {
            fprintf(stderr, "Failed to create kernel!\n");
            return false;
        }
        /* OpenCL Kernel - end */


        /* OpenCL Command queue - begin */
        command_queue =  clCreateCommandQueue(context, device_id, 0, NULL);
        if (!command_queue) {
            fprintf(stderr, "Failed to create command_queue!\n");
            return false;
        }
        /* OpenCL Command queue - end */


        /* Create OpenGL texture and share it - begin */
        m_Texture = Texture::CreateEmpty(1024, 640, 4);
        image_object = clCreateFromGLTexture(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, m_Texture->GetHandle(), NULL);
        if (!image_object) {
            fprintf(stderr, "Failed to create shared texture between OpenGL and OpenCL\n");
            return false;
        };
        /* Create OpenGL texture and share it - end */

        return m_Texture;
    }

    void Renderer::Draw() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT);)
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f);)
        glFinish();

        
        if (clEnqueueAcquireGLObjects(command_queue, 1, &image_object, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error Enquering GL Objects!\n");
        }

        size_t global_work_size[] = {(size_t)m_Texture->GetWidth(), (size_t)m_Texture->GetHeight()};

        if (clSetKernelArg(kernel, 0, sizeof(cl_mem), &image_object) != CL_SUCCESS) {
            fprintf(stderr, "Error setting kernel argument!\n");
        }

        //fprintf(stderr, "Size %d, %u\n",  m_Texture->GetWidth(), global_work_size[1]);
        if (clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL) == CL_INVALID_GLOBAL_WORK_SIZE) {
            fprintf(stderr, "Error running kernel!\n");
        }

        if (clEnqueueReleaseGLObjects(command_queue, 1, &image_object, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error releasing GL Objects!\n");
        }
        clFinish(command_queue);
    }

    Renderer::~Renderer() {
        
        if (image_object) {
            clReleaseMemObject(image_object);
        }

        if (command_queue) {
            clReleaseCommandQueue(command_queue);
        }

        if (kernel) {
            clReleaseKernel(kernel);
        }

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
            delete[] device_name;
        }

        if (platform_name) {
            delete[] platform_name;
        }
    }

}