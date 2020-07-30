#pragma once

#include <glm/glm.hpp>

#include "../Core/types.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "ImageExtension.h"

namespace fbmgen {

    class Application;

    class Renderer {
    public:
        inline const Texture* GetTexture() const { return m_Texture; }

        bool Create(Application* app);
        void Draw();

        inline void SetCamera(Camera* camera) { m_Camera = camera; }

        // Move to Controller
        inline glm::vec3 GetSunDirection() const { return m_SunDirection; }
        inline void SetSunDirection(const glm::vec3& dir) { m_SunDirection = dir; }
        void SetSunPosition(const glm::vec2& imageCoord, const glm::vec2& resolution);

        inline void SetSunColor(const glm::vec3& color) { m_SunColor = color; }
        inline glm::vec3 GetSunColor() const { return m_SunColor; }

        inline void SetSunIntensity(f32 value) { m_SunIntensity = value; }
        inline f32 GetSunIntensity() const { return m_SunIntensity; }

        inline void SetSunSize(f32 value) { m_SunSize = value; }
        inline f32 GetSunSize() const { return m_SunSize; }

        inline void SetFbmOctaves(s32 value) { m_FbmOctaves = value; }
        inline s32 GetFbmOctaves() const { return m_FbmOctaves; }

        inline void SetFbmScale(f32 value) { m_FbmScale = value; }
        inline f32 GetFbmScale() const { return m_FbmScale; }
        //

        void RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality = 100);
        ~Renderer();
    private:
        Application* m_App = nullptr;
        Shader* m_Shader = nullptr;
        Texture* m_Texture = nullptr;
        Camera* m_Camera = nullptr;

        /* Parameters */
        glm::vec3 m_SunDirection = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));
        glm::vec3 m_SunColor = glm::vec3(1.0f, 0.855f, 0.740f);
        f32 m_SunIntensity = 1.2f;
        f32 m_SunSize = 5.0f;
        f32 m_FbmScale = 1.0f;
        s32 m_FbmOctaves = 16;
        
        /* Screen Quad */ 
        u32 m_FrameBuffer = 0;
        u32 m_VertexBuffer = 0;
        u32 m_IndexBuffer = 0;
        u32 m_VertexArray = 0;
    };
}