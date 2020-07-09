#include <windows.h>



#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../Core/fbmgen_gl.h"
#include "../Core/Timer.h"
#include "../Core/utils.h"

#include "ImageExtension.h"
#include "../Core/Application.h"

namespace fbmgen {

    bool Renderer::Create(Application* app) {
        if (!app) {
            return false;
        }

        m_App = app;
        auto& log = m_App->GetLog();
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
        char compiler_options[256] = {'\0'};
        GetAbsolutePath(path_to_program, MAX_PATH);

        strncat(compiler_options, "-Werror -I ", 256 - strlen(compiler_options) - 1);
        strncat(compiler_options, path_to_program, 256 - strlen(compiler_options) - 1);
        strncat(compiler_options, "resources/kernel", 256 - strlen(compiler_options) - 1);
        strncat(path_to_program, "resources/kernel/terrain.cl", MAX_PATH - strlen(path_to_program) - 1);

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
       
        const char* options = compiler_options;
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

        log.AddLog("Renderer inited sucessfully\n");
        return m_Texture;
    }

    void Renderer::Draw() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT);)
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f);)
        glFinish();

        
        if (clEnqueueAcquireGLObjects(command_queue, 1, &image_object, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error Enquering GL Objects!\n");
            exit(EXIT_FAILURE);
        }

        glm::vec3 right = m_Camera->GetRight();
        glm::vec3 up = m_Camera->GetUp();
        glm::vec3 front = m_Camera->GetFront();
        glm::vec3 pos = m_Camera->position;

        CameraInfo cameraInfo;
        cameraInfo.position = {{pos.x, pos.y, pos.z}};
        cameraInfo.right = {{right.x, right.y, right.z}};
        cameraInfo.up = {{up.x, up.y, up.z}};
        cameraInfo.front = {{front.x, front.y, front.z}};

        size_t global_work_size[] = {(size_t)m_Texture->GetWidth(), (size_t)m_Texture->GetHeight()};

        if (clSetKernelArg(kernel, 0, sizeof(cl_mem), &image_object) != CL_SUCCESS) {
            fprintf(stderr, "Error setting kernel argument!\n");
            exit(EXIT_FAILURE);
        }

        if (clSetKernelArg(kernel, 1, sizeof(CameraInfo), &cameraInfo) != CL_SUCCESS) {
            fprintf(stderr, "Error setting kernel argument!\n");
            exit(EXIT_FAILURE);
        }

        //fprintf(stderr, "Size %d, %u\n",  m_Texture->GetWidth(), global_work_size[1]);
        if (clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error running kernel!\n");
            exit(EXIT_FAILURE);
        }

        if (clEnqueueReleaseGLObjects(command_queue, 1, &image_object, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Error releasing GL Objects!\n");
            exit(EXIT_FAILURE);
        }
        clFinish(command_queue);
    }

    void Renderer::RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality) {
        auto& log = m_App->GetLog();

        Timer timer;
        timer.Run();

        /* Create image object */
        cl_mem output_image = 0;
        cl_image_format format[] = { CL_RGBA, CL_FLOAT };
        cl_image_desc desc;
        desc.image_type = CL_MEM_OBJECT_IMAGE2D;
        desc.image_width = width;
        desc.image_height = height;
        desc.image_array_size = 0;
        desc.image_row_pitch = 0;
        desc.image_slice_pitch = 0;
        desc.num_mip_levels = 0;
        desc.num_samples = 0;

        output_image = clCreateImage(context, CL_MEM_WRITE_ONLY, format, &desc, NULL, NULL);

        if (!output_image) {
            fprintf(stderr, "Failed to create OpenCL image!\n");
            return;
        }

        glm::vec3 right = m_Camera->GetRight();
        glm::vec3 up = m_Camera->GetUp();
        glm::vec3 front = m_Camera->GetFront();
        glm::vec3 pos = m_Camera->position;

        CameraInfo cameraInfo;
        cameraInfo.position = {{pos.x, pos.y, pos.z}};
        cameraInfo.right = {{right.x, right.y, right.z}};
        cameraInfo.up = {{up.x, up.y, up.z}};
        cameraInfo.front = {{front.x, front.y, front.z}};

        /* Run kernel */
        if (clSetKernelArg(kernel, 0, sizeof(cl_mem), &output_image) != CL_SUCCESS) {
            fprintf(stderr, "Error setting kernel argument!\n");
            return;
        }

        if (clSetKernelArg(kernel, 1, sizeof(CameraInfo), &cameraInfo) != CL_SUCCESS) {
            fprintf(stderr, "Error setting kernel argument!\n");
            return;
        }

        size_t global_work_size[] = {(size_t)width, (size_t)height};
        if (clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Failed to render result!\n");
            return;
        }

        clFinish(command_queue);

        /* Write result to file */

        cl_float4* data = static_cast<cl_float4*>(malloc(sizeof(cl_float4)*width*height));
        u8* buffer = static_cast<u8*>(malloc(sizeof(u8)*width*height*4));

        
        size_t origin[] = {0, 0, 0};
        size_t region[] = {(size_t)width, (size_t)height, 1};
        if (clEnqueueReadImage(command_queue, output_image, CL_TRUE, origin, region, 0, 0, data, 0, NULL, NULL) != CL_SUCCESS) {
            fprintf(stderr, "Failed to read image from OpenCL!\n");
            return;
        }

        for (int j = 0; j < height; j++) {
		    for (int i = 0; i < width; i++) {
			    cl_float4 item = data[((long int)height - 1 - j)* width + i];
			    buffer[(long int)width*4*j + 4*i + 0] = static_cast<u8>(item.s[0] * 255.0f);
			    buffer[(long int)width*4*j + 4*i + 1] = static_cast<u8>(item.s[1] * 255.0f);
			    buffer[(long int)width*4*j + 4*i + 2] = static_cast<u8>(item.s[2] * 255.0f);
			    buffer[(long int)width*4*j + 4*i + 3] = static_cast<u8>(item.s[3] * 255.0f);
		    }
	    }

        switch (extension) {
            case ImageExtension::Png: {
                stbi_write_png(path, width, height, 4, buffer, width * 4 * sizeof(u8));
                break;
            }
            case ImageExtension::Bmp: {
                stbi_write_bmp(path, width, height, 4, buffer);
                break;
            }
            case ImageExtension::Jpeg: {
                stbi_write_jpg(path, width, height, 4, buffer, quality);
                break;
            }
        }
        
        free(data);
        free(buffer);

        timer.Stop();

        log.AddLog("%s is rendered in %lf seconds\n", path, timer.GetTimeS());
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