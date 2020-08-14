#pragma once

#include <glm/glm.hpp>

#include "../Core/types.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "ImageExtension.h"

namespace fbmgen {

    class Application;

    enum class FbmInterpolation {
        Cubic = 0,
        Quintic = 1
    };


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

        inline void SetFbmAmplitude(f32 value) { m_FbmAmplitude = value; }
        inline f32 GetFbmAmplitude() const { return m_FbmAmplitude; }

        inline void SetFbmFrequency(f32 value) { m_FbmFrequency = value; }
        inline f32 GetFbmFrequency() const { return m_FbmFrequency; }

        inline void SetFbmSmoothness(f32 value) { m_FbmSmoothness = value; }
        inline f32 GetFbmSmoothness() const { return m_FbmSmoothness; }

        void SetFbmInterpolation(FbmInterpolation interp);
        inline FbmInterpolation GetFbmInterpolation() const { return m_FbmInterpolation; }

        void SetFbmUseDerivatives(bool value);
        inline bool GetFbmUseDerivatives() const { return m_FbmUseDerivatives; }

        inline void SetTerrainHeight(f32 value) { m_TerrainHeight = value; }
        inline f32 GetTerrainHeight() const { return m_TerrainHeight; }

        inline void SetSnowLevel(f32 value) { m_SnowLevel = value; }
        inline f32 GetSnowLevel() const { return m_SnowLevel; }

        inline void SetGrassLevel(f32 value) { m_GrassLevel = value; }
        inline f32 GetGrassLevel() const { return m_GrassLevel; }

        void SetWaterEnabled(bool value);
        inline bool GetWaterEnabled() const { return m_WaterEnabled; }

        inline void SetWaterLevel(f32 value) { m_WaterLevel = value; }
        inline f32 GetWaterLevel() const { return m_WaterLevel; }
        //

        void RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality = 100);
        void SaveConfig(const char* path);
        void LoadConfig(const char* path);
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
        f32 m_FbmAmplitude = 1.0f;
        f32 m_FbmFrequency = 0.050f;
        s32 m_FbmOctaves = 16;
        f32 m_FbmSmoothness = 1.0f;
        f32 m_TerrainHeight = 10.0f;
        f32 m_SnowLevel = 1.4f;
        f32 m_GrassLevel = 0.5f;
        f32 m_WaterLevel = 0.4f;


        FbmInterpolation m_FbmInterpolation = FbmInterpolation::Quintic;
        bool m_FbmUseDerivatives = false;
        bool m_WaterEnabled = false;

        
        /* Screen Quad */ 
        u32 m_FrameBuffer = 0;
        u32 m_VertexBuffer = 0;
        u32 m_IndexBuffer = 0;
        u32 m_VertexArray = 0;
    };
}