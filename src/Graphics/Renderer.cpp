#include <windows.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cjson/cJSON.h>

#include <glm/gtc/type_ptr.hpp>

#include "../Core/Timer.h"
#include "../Core/fbmgen_gl.h"
#include "../Core/utils.h"
#include "../Core/Application.h"
#include "ImageExtension.h"

namespace fbmgen {

    void Renderer::SetSunPosition(const glm::vec2& imageCoord, const glm::vec2& resolution) {
        glm::vec2 xy = imageCoord - resolution*0.5f;
        float z = resolution.y / tan(glm::radians(60.0f) / 2.0f);
        glm::vec3 direction = glm::normalize(xy.x * m_Camera->GetRight() - xy.y * m_Camera->GetUp() + z*m_Camera->GetFront());
        if (direction.y < -0.1f) {
            direction.y = -0.1f;
            glm::normalize(direction);
        }
        m_SunDirection = glm::normalize(direction);
    }

    void Renderer::SetFbmInterpolation(FbmInterpolation interpolation) {
        m_FbmInterpolation = interpolation;
        auto& console = m_App->GetLog();
        m_Shader->ClearDefines();
        
        if (m_FbmUseDerivatives) {
            m_Shader->AddDefine("\n#define USE_DERIVATIVES\n");
        }

        if (m_WaterEnabled) {
            m_Shader->AddDefine("\n#define WATER_ENABLED\n");
        }

        switch (interpolation) {
            case FbmInterpolation::Quintic: {
                m_Shader->AddDefine("\n#define QUINTIC_POLYNOMIAL\n");
                break;
            }
            case FbmInterpolation::Cubic: {
                m_Shader->AddDefine("\n#define CUBIC_POLYNOMIAL\n");
                break;
            }
        }
        
        Timer t;
        t.Run();
        assert(m_Shader->Compile());
        t.Stop();
        console.AddLog("Shader recompiled in %g seconds\n", t.GetTimeS());
    };

    void Renderer::SetFbmUseDerivatives(bool flag) {
        m_FbmUseDerivatives = flag;
        auto& console = m_App->GetLog();
        m_Shader->ClearDefines();
        
        if (m_FbmUseDerivatives) {
            m_Shader->AddDefine("\n#define USE_DERIVATIVES\n");
        }

        if (m_WaterEnabled) {
            m_Shader->AddDefine("\n#define WATER_ENABLED\n");
        }

        switch (m_FbmInterpolation) {
            case FbmInterpolation::Quintic: {
                m_Shader->AddDefine("\n#define QUINTIC_POLYNOMIAL\n");
                break;
            }
            case FbmInterpolation::Cubic: {
                m_Shader->AddDefine("\n#define CUBIC_POLYNOMIAL\n");
                break;
            }
        }
        
        Timer t;
        t.Run();
        assert(m_Shader->Compile());
        t.Stop();
        console.AddLog("Shader recompiled in %g seconds\n", t.GetTimeS());
    };

    void Renderer::SetWaterEnabled(bool flag) {
        m_WaterEnabled = flag;
        auto& console = m_App->GetLog();
        m_Shader->ClearDefines();
        
        if (m_FbmUseDerivatives) {
            m_Shader->AddDefine("\n#define USE_DERIVATIVES\n");
        }

        if (m_WaterEnabled) {
            m_Shader->AddDefine("\n#define WATER_ENABLED\n");
        }

        switch (m_FbmInterpolation) {
            case FbmInterpolation::Quintic: {
                m_Shader->AddDefine("\n#define QUINTIC_POLYNOMIAL\n");
                break;
            }
            case FbmInterpolation::Cubic: {
                m_Shader->AddDefine("\n#define CUBIC_POLYNOMIAL\n");
                break;
            }
        }
        
        Timer t;
        t.Run();
        assert(m_Shader->Compile());
        t.Stop();
        console.AddLog("Shader recompiled in %g seconds\n", t.GetTimeS());
    };

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

        m_Shader->ClearDefines();
        switch (m_FbmInterpolation) {
            case FbmInterpolation::Quintic: {
                m_Shader->AddDefine("\n#define QUINTIC_POLYNOMIAL\n");
                break;
            }
            case FbmInterpolation::Cubic: {
                m_Shader->AddDefine("\n#define CUBIC_POLYNOMIAL\n");
                break;
            }
        }
        
        if (!m_Shader->Compile()) {
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
        log.AddLog("Renderer inited sucessfully\n");
        return true;
    }

    void Renderer::Draw() {
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        GLCALL(glViewport(0, 0, m_Texture->GetWidth(),m_Texture->GetHeight()));

        m_Shader->Run();
        m_Shader->SetUniform("u_ViewMatrix", m_Camera->GetViewMatrix());
        m_Shader->SetUniform("u_SunDirection", m_SunDirection);
        m_Shader->SetUniform("u_SunColor", m_SunColor);
        m_Shader->SetUniform("u_Resolution", glm::vec2(m_Texture->GetWidth(), m_Texture->GetHeight()));
        m_Shader->SetUniform("u_SunIntensity", m_SunIntensity);
        m_Shader->SetUniform("u_SunSize", m_SunSize);
        m_Shader->SetUniform("u_FbmOctaves", m_FbmOctaves);
        m_Shader->SetUniform("u_FbmFrequency", m_FbmFrequency);
        m_Shader->SetUniform("u_FbmAmplitude", m_FbmAmplitude);
        if (m_FbmUseDerivatives)
             m_Shader->SetUniform("u_FbmSmoothness", m_FbmSmoothness);
        m_Shader->SetUniform("u_TerrainHeight", m_TerrainHeight);
        m_Shader->SetUniform("u_SnowLevel", m_SnowLevel);
        m_Shader->SetUniform("u_GrassLevel", m_GrassLevel);
        if (m_WaterEnabled) {
            m_Shader->SetUniform("u_WaterLevel", m_WaterLevel);
            m_Shader->SetUniform("u_Time", (f32)glfwGetTime());
        }

        GLCALL(glBindVertexArray(m_VertexArray));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer));

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void Renderer::RenderImage(const char* path, s32 width, s32 height, ImageExtension extension, s32 quality) {
        auto& log = m_App->GetLog();

        Timer timer;
        timer.Run();
        
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer));
        s32 old_width = m_Texture->GetWidth();
        s32 old_height = m_Texture->GetHeight();

        m_Texture->Resize(width, height);
        
        GLCALL(glViewport(0, 0, width, height));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

        m_Shader->Run();
        m_Shader->SetUniform("u_ViewMatrix", m_Camera->GetViewMatrix());
        m_Shader->SetUniform("u_SunDirection", m_SunDirection);
        m_Shader->SetUniform("u_Resolution", glm::vec2(width, height));
        m_Shader->SetUniform("u_SunColor", m_SunColor);
        m_Shader->SetUniform("u_SunIntensity", m_SunIntensity);
        m_Shader->SetUniform("u_SunSize", m_SunSize);
        m_Shader->SetUniform("u_FbmOctaves", m_FbmOctaves);
        m_Shader->SetUniform("u_FbmFrequency", m_FbmFrequency);
        m_Shader->SetUniform("u_FbmAmplitude", m_FbmAmplitude);
        if (m_FbmUseDerivatives)
            m_Shader->SetUniform("u_FbmSmoothness", m_FbmSmoothness);
        m_Shader->SetUniform("u_TerrainHeight", m_TerrainHeight);
        m_Shader->SetUniform("u_SnowLevel", m_SnowLevel);
        m_Shader->SetUniform("u_GrassLevel", m_GrassLevel);
        if (m_WaterEnabled) {
            m_Shader->SetUniform("u_WaterLevel", m_WaterLevel);
            m_Shader->SetUniform("u_Time", (f32)glfwGetTime());
        }

        GLCALL(glBindVertexArray(m_VertexArray));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer));

        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

        u8* buffer = (u8*)malloc(sizeof(u8)*width*height*4);
        GLCALL(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
        stbi_flip_vertically_on_write(true);
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

        free(buffer);
        m_Texture->Resize(old_width, old_height);
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        timer.Stop();
        log.AddLog("%s is rendered in %lf seconds\n", path, timer.GetTimeS());
    }

    void Renderer::SaveConfig(const char* path) {
        auto& log = m_App->GetLog();
        cJSON* config = cJSON_CreateObject();
        if (!config) {
            log.AddLog("Failed to save config file\n");
            return;
        }

        cJSON* camera = cJSON_CreateObject();
        if (!camera) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(config, "Camera", camera);

        /* Camera */
        glm::mat4 view = m_Camera->GetViewMatrix();
        cJSON* v = cJSON_CreateFloatArray(glm::value_ptr(view), 16);
        if (!v) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(camera, "View Matrix", v);

        /* Yaw */
        cJSON* yaw = cJSON_CreateNumber(m_Camera->GetYaw());
        if (!yaw) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(camera, "Yaw", yaw);

        /* Pitch */
        cJSON* pitch = cJSON_CreateNumber(m_Camera->GetPitch());
        if (!pitch) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(camera, "Pitch", pitch);

        cJSON* sun = cJSON_CreateObject();
        if (!sun) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(config, "Sun", sun);

        /* Sun direction */
        cJSON* sun_direction = cJSON_CreateFloatArray(glm::value_ptr(glm::normalize(m_SunDirection)), 3);
        if (!sun_direction) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(sun, "Sun Direction", sun_direction);

        /* Sun color */
        cJSON* sun_color = cJSON_CreateFloatArray(glm::value_ptr(m_SunColor), 3);
        if (!sun_color) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(sun, "Sun Color", sun_color);

        /* Sun intensity */
        cJSON* sun_intensity = cJSON_CreateNumber(m_SunIntensity);
        if (!sun_intensity) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(sun, "Sun Intensity", sun_intensity);

        /* Sun size */
        cJSON* sun_size = cJSON_CreateNumber(m_SunSize);
        if (!sun_size) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(sun, "Sun Size", sun_size);


        cJSON* fbm = cJSON_CreateObject();
        if (!fbm) {
            log.AddLog("Failed to save config file\n");
            return;
        }

        cJSON_AddItemToObject(config, "FBM", fbm);

        /* FBM amplitude */
        cJSON* fbm_amplitude = cJSON_CreateNumber(m_FbmAmplitude);
        if (!fbm_amplitude) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(fbm, "Amplitude", fbm_amplitude);

        /* FBM amplitude */
        cJSON* fbm_frequency = cJSON_CreateNumber(m_FbmFrequency);
        if (!fbm_frequency) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(fbm, "Frequency", fbm_frequency);

        /* FBM octaves */
        cJSON* fbm_octaves = cJSON_CreateNumber(m_FbmOctaves);
        if (!fbm_octaves) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(fbm, "Octaves", fbm_octaves);

        /* FBM use derivatives*/
        cJSON* fbm_use_derivatives = cJSON_CreateBool(m_FbmUseDerivatives);
        if (!fbm_use_derivatives) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(fbm, "Use Derivatives", fbm_use_derivatives);

        /* FBM smoothness*/
        cJSON* fbm_smoothness = cJSON_CreateNumber(m_FbmSmoothness);
        if (!fbm_smoothness) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(fbm, "Smoothness", fbm_smoothness);

        cJSON* terrain = cJSON_CreateObject();
        if (!terrain) {
            log.AddLog("Failed to save config file\n");
            return;
        }

        cJSON_AddItemToObject(config, "Terrain", terrain);

         /* Terrain height */
        cJSON* terrain_height = cJSON_CreateNumber(m_TerrainHeight);
        if (!terrain_height) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(terrain, "Height", terrain_height);

        /* Snow level */
        cJSON* snow_level = cJSON_CreateNumber(m_SnowLevel);
        if (!snow_level) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(terrain, "Snow Level", snow_level);

        /* Grass level */
        cJSON* grass_level = cJSON_CreateNumber(m_GrassLevel);
        if (!grass_level) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(terrain, "Grass Level", grass_level);

        /* Water enabled */
        cJSON* water_enabled = cJSON_CreateBool(m_WaterEnabled);
        if (!water_enabled) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(terrain, "Water Enabled", water_enabled);

        /* Water level */
        cJSON* water_level = cJSON_CreateNumber(m_WaterLevel);
        if (!water_level) {
            log.AddLog("Failed to save config file\n");
            return;
        }
        cJSON_AddItemToObject(terrain, "Water Level", water_level);

        /* Writing file */
        FILE* config_file = fopen(path, "wb");
        if (!config_file) {
            log.AddLog("Failed to save config file\n");
            return;
        }

        fprintf(config_file, "%s", cJSON_Print(config));
        fclose(config_file);
        cJSON_Delete(config);
    }

    void Renderer::LoadConfig(const char* path) {
        auto& log = m_App->GetLog();

        char* config_str;
        size_t size;

        if (!ReadFileToString(path, &config_str, &size)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        cJSON *config = cJSON_Parse(config_str);

        if (!config) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        cJSON* camera = cJSON_GetObjectItemCaseSensitive(config, "Camera");
        if (!camera) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        /* View matrix */
        cJSON* view = cJSON_GetObjectItemCaseSensitive(camera, "View Matrix");
        if (!view || !cJSON_IsArray(view) || !(cJSON_GetArraySize(view) == 16)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        f32 tmp[16];
        for (int i = 0; i < 16; i++) {
            cJSON* value = cJSON_GetArrayItem(view, i);
            if (!cJSON_IsNumber(value)) {
                log.AddLog("Failed to load config %s\n", path);
                return;
            }
            tmp[i] = (f32)value->valuedouble;
        }
        

        /* Yaw */
        cJSON* yaw = cJSON_GetObjectItemCaseSensitive(camera, "Yaw");
        if (!yaw || !cJSON_IsNumber(yaw)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        /* Pitch */
        cJSON* pitch = cJSON_GetObjectItemCaseSensitive(camera, "Pitch");
        if (!pitch || !cJSON_IsNumber(pitch)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        


        cJSON* sun = cJSON_GetObjectItemCaseSensitive(config, "Sun");
        if (!sun) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        /* Sun direction */
        glm::vec3 tmp_sun_direction;
        cJSON* sun_direction = cJSON_GetObjectItemCaseSensitive(sun, "Sun Direction");
        if (!sun_direction || !cJSON_IsArray(sun_direction) || !(cJSON_GetArraySize(sun_direction) == 3)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        for (int i = 0; i < 3; i++) {
            cJSON* value = cJSON_GetArrayItem(sun_direction, i);
            if (!cJSON_IsNumber(value)) {
                log.AddLog("Failed to load config %s\n", path);
                return;
            }
            tmp_sun_direction[i] = (f32)value->valuedouble;
        }

        /* Sun color */
        glm::vec3 tmp_sun_color;
        cJSON* sun_color = cJSON_GetObjectItemCaseSensitive(sun, "Sun Color");
        if (!sun_color || !cJSON_IsArray(sun_color) || !(cJSON_GetArraySize(sun_color) == 3)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        for (int i = 0; i < 3; i++) {
            cJSON* value = cJSON_GetArrayItem(sun_color, i);
            if (!cJSON_IsNumber(value)) {
                log.AddLog("Failed to load config %s\n", path);
                return;
            }
            tmp_sun_color[i] = (f32)value->valuedouble;
        }

        /* Sun intensity */
        cJSON* sun_intensity = cJSON_GetObjectItemCaseSensitive(sun, "Sun Intensity");
        if (!sun_intensity || !cJSON_IsNumber(sun_intensity)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        /* Sun size */
        cJSON* sun_size = cJSON_GetObjectItemCaseSensitive(sun, "Sun Size");
        if (!sun_size || !cJSON_IsNumber(sun_size)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        cJSON* fbm = cJSON_GetObjectItemCaseSensitive(config, "FBM");
        if (!fbm) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        /* FBM amplitude */
        cJSON* fbm_amplitude = cJSON_GetObjectItemCaseSensitive(fbm, "Amplitude");
        if (!fbm_amplitude || !cJSON_IsNumber(fbm_amplitude)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        /* FBM frequency */
        cJSON* fbm_frequency = cJSON_GetObjectItemCaseSensitive(fbm, "Frequency");
        if (!fbm_frequency || !cJSON_IsNumber(fbm_frequency)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       
        /* FBM octaves */
        cJSON* fbm_octaves = cJSON_GetObjectItemCaseSensitive(fbm, "Octaves");
        if (!fbm_octaves || !cJSON_IsNumber(fbm_octaves)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       

        /* FBM use derivatives */
        cJSON* fbm_use_derivatives = cJSON_GetObjectItemCaseSensitive(fbm, "Use Derivatives");
        if (!fbm_use_derivatives || !cJSON_IsBool(fbm_use_derivatives)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       

        /* FBM smoothness */
        cJSON* fbm_smoothness = cJSON_GetObjectItemCaseSensitive(fbm, "Smoothness");
        if (!fbm_smoothness || !cJSON_IsNumber(fbm_smoothness)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       

        cJSON* terrain = cJSON_GetObjectItemCaseSensitive(config, "Terrain");
        if (!fbm) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }

        /* Terrain height */
        cJSON* terrain_height = cJSON_GetObjectItemCaseSensitive(terrain, "Height");
        if (!terrain_height || !cJSON_IsNumber(terrain_height)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       

        /* Snow level */
        cJSON* snow_level = cJSON_GetObjectItemCaseSensitive(terrain, "Snow Level");
        if (!snow_level || !cJSON_IsNumber(snow_level)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        

        /* Grass level */
        cJSON* grass_level = cJSON_GetObjectItemCaseSensitive(terrain, "Grass Level");
        if (!grass_level || !cJSON_IsNumber(grass_level)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        /* Water enabled */
        cJSON* water_enabled = cJSON_GetObjectItemCaseSensitive(terrain, "Water Enabled");
        if (!water_enabled || !cJSON_IsBool(water_enabled)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
       

        /* Water level */
        cJSON* water_level = cJSON_GetObjectItemCaseSensitive(terrain, "Water Level");
        if (!water_level || !cJSON_IsNumber(water_level)) {
            log.AddLog("Failed to load config %s\n", path);
            return;
        }
        
        m_Camera->SetViewMatrix(glm::make_mat4(tmp));
        m_Camera->SetYaw((f32)yaw->valuedouble);
        m_Camera->SetPitch((f32)pitch->valuedouble);

        m_SunDirection = tmp_sun_direction;
        m_SunColor = tmp_sun_color;
        m_SunIntensity = (f32)sun_intensity->valuedouble;
        m_SunSize = (f32)sun_size->valuedouble;

        m_FbmAmplitude = (f32)fbm_amplitude->valuedouble;
        m_FbmFrequency = (f32)fbm_frequency->valuedouble;
        m_FbmOctaves = (s32)fbm_octaves->valuedouble;
        SetFbmUseDerivatives(cJSON_IsTrue(fbm_use_derivatives));
        m_FbmSmoothness = (f32)fbm_smoothness->valuedouble;
        m_TerrainHeight = (f32)terrain_height->valuedouble;
        m_SnowLevel = (f32)snow_level->valuedouble;
        m_GrassLevel = (f32)grass_level->valuedouble;
        SetWaterEnabled(cJSON_IsTrue(water_enabled));
        m_WaterLevel = (f32)water_level->valuedouble;

        free(config_str);
        cJSON_Delete(config);
    }

    Renderer::~Renderer() {

        if (m_Shader) {
            delete m_Shader;
        }

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