#include <windows.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../Core/Timer.h"
#include "../Core/fbmgen_gl.h"
#include "../Core/utils.h"
#include "../Core/Application.h"
#include "ImageExtension.h"
// #include "Shader.h"
// #include "Camera.h"
// #include "Texture.h"

namespace fbmgen {

    void Renderer::SetSunPosition(const glm::vec2& imageCoord, const glm::vec2& resolution) {

        glm::vec2 xy = imageCoord - resolution*0.5f;
        float z = resolution.y / tan(glm::radians(45.0f) / 2.0f);
        glm::vec3 direction = glm::normalize(xy.x * m_Camera->GetRight() - xy.y * m_Camera->GetUp() + z*m_Camera->GetFront());
        m_SunDirection = glm::normalize(direction);
    }

    bool Renderer::Create(Application* app) {
        if (!app) {
            return false;
        }

        m_App = app;
        auto& log = m_App->GetLog();
        
        // GLEW
        glewExperimental = true; 
        if (glewInit() != GLEW_OK) { 
            return false;
        }

        f32 vertices[] = { 
                            -1.0f,  1.0f, 0.0f, 1.0f,
                            -1.0f, -1.0f, 0.0f, 0.0f,
                             1.0f, -1.0f, 1.0f, 0.0f,
                             1.0f,  1.0f, 1.0f, 1.0f
                         };

        u32 indices[] = { 
                            0, 1, 2,
                            0, 2, 3
                        };
        
        GLCALL(glGenBuffers(1, &m_VertexBuffer));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

        GLCALL(glGenBuffers(1, &m_IndexBuffer));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GLCALL(glGenVertexArrays(1, &m_VertexArray));
        GLCALL(glBindVertexArray(m_VertexArray));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer));
        GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(f32), (void *)0));
        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(f32), (void*)(2 * sizeof(f32))));
        GLCALL(glEnableVertexAttribArray(1));


        char path_to_shader[MAX_PATH];
        if (!GetAbsolutePath(path_to_shader, MAX_PATH)) {
            return false;
        }

        std::string filename = std::string(path_to_shader) + "resources/shader/terrain.glsl"; 
        m_Shader = Shader::Load(filename.c_str());
        if (!m_Shader) {
            return false;
        }

        /* Create framebuffer and attach m_Texture to it */
        GLCALL(glGenFramebuffers(1, &m_FrameBuffer));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer));

        m_Texture = Texture::CreateEmpty(1024, 640, 4);
        if (!m_Texture) {
            return false;
        }
        m_Texture->SetFilterMode(Texture::FilterMode::Linear);
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetHandle(), 0));  

        GLenum frame_buffer_status;
        GLCALL(frame_buffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if (frame_buffer_status != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "Framebuffer is not complete!\n");
            return false;
        }
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


       /* char path_to_program[MAX_PATH];
        char compiler_options[256] = {'\0'};
       

        strncat(compiler_options, "-Werror -I ", 256 - strlen(compiler_options) - 1);
        strncat(compiler_options, path_to_program, 256 - strlen(compiler_options) - 1);
        strncat(compiler_options, "resources/kernel", 256 - strlen(compiler_options) - 1);
        strncat(path_to_program, "resources/kernel/terrain.cl", MAX_PATH - strlen(path_to_program) - 1);

        if (!ReadFileToString(path_to_program, &source_string, &source_string_size)) {
            return false;
        }*/

        log.AddLog("Renderer inited sucessfully\n");
        return true;
    }

    void Renderer::Draw() {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        glm::ivec2 previewSize = m_App->GetGui().GetPreviewSize();
        GLCALL(glViewport(0.0f, 0.0f, previewSize.x, previewSize.y));

        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        m_Shader->Run();
        m_Shader->SetUniform("u_ViewMatrix", m_Camera->GetViewMatrix());
        m_Shader->SetUniform("u_SunDirection", m_SunDirection);
        m_Shader->SetUniform("u_Resolution", glm::vec2(m_Texture->GetWidth(), m_Texture->GetHeight()));
        m_Shader->SetUniform("u_SunIntensity", m_SunIntensity);
        GLCALL(glBindVertexArray(m_VertexArray));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer));

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void Renderer::RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality) {
        auto& log = m_App->GetLog();

        Timer timer;
        timer.Run();
        /* do stuff here */
        timer.Stop();
        log.AddLog("%s is rendered in %lf seconds\n", path, timer.GetTimeS());
    }

    Renderer::~Renderer() {
        if (m_Texture) {
            delete m_Texture;
        }

        if (m_FrameBuffer) {
            GLCALL(glDeleteFramebuffers(1, &m_FrameBuffer));
        }

        if (m_VertexArray) {
            GLCALL(glDeleteVertexArrays(1, &m_VertexArray));
        }

        if (m_IndexBuffer) {
            GLCALL(glDeleteBuffers(1, &m_IndexBuffer));
        }

        if (m_VertexBuffer) {
            GLCALL(glDeleteBuffers(1, &m_VertexBuffer));
        }
    }
}