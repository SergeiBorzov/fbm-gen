#include <cstdio>

#include <glm/gtc/type_ptr.hpp>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../Core/types.h"
#include "../Core/Application.h"
#include "../Input/Input.h"
#include "../Graphics/Camera.h"
#include "../Graphics/ImageExtension.h"

namespace fbmgen {
    bool Gui::Create(Application* app) {
        if (app == nullptr) {
            return false;
        }

        m_App = app;
        auto& log = m_App->GetLog();
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO(); (void)io;
        // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
        // Setup Dear ImGui style

        ImGuiStyle &st = ImGui::GetStyle();
        st.FrameBorderSize = 1.0f;
        st.FramePadding = ImVec2(4.0f,2.0f);
        st.ItemSpacing = ImVec2(8.0f,2.0f);
        st.WindowBorderSize = 1.0f;
        st.TabBorderSize = 1.0f;
        st.WindowRounding = 1.0f;
        st.ChildRounding = 1.0f;
        st.FrameRounding = 1.0f;
        st.ScrollbarRounding = 1.0f;
        st.GrabRounding = 1.0f;
        st.TabRounding = 1.0f;

        // Setup style
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
        colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
        colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
        colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
        colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        ImGui_ImplGlfw_InitForOpenGL(m_App->GetWindow().GetPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 130");

        m_FileExplorerLoadConfig.SetTitle("Open config file");
        m_FileExplorerLoadConfig.SetTitle("Save config file");
        m_FileExplorerRender.SetTitle("Save image as");

        std::vector<const char*> image_filter = { ".jpg", ".jpeg", ".png", ".bmp" };
        m_FileExplorerRender.SetTypeFilters(image_filter);
        
        log.AddLog("Gui inited sucessfully\n");
        return true;
    }

    void Gui::MenuBar() {
        auto& window = m_App->GetWindow();
        if(ImGui::BeginMainMenuBar()) {
            
            if (ImGui::BeginMenu("File")) {
                ImGui::Separator();
                
                if (ImGui::MenuItem("Open", "Ctrl + O") || 
                    Input::GetKey(KeyCode::LeftCtrl)) {
                    m_FileExplorerLoadConfig.Open();
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl + S")) {
                    m_FileExplorerSaveConfig.Open();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt + F4")) {
                    window.Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Render Image", "F5")) {
                    m_RenderSettingsVisible = true;
                    s32 width;
                    s32 height;
                    window.GetSize(&width, &height);
                    ImGui::SetNextWindowPos(ImVec2(width*0.5f, height*0.5f));
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void Gui::Preview() {
        auto& renderer = m_App->GetRenderer();
        auto& window = m_App->GetWindow();

        ImGui::SetNextWindowPos(ImVec2(0, 18));
        s32 width, height;
        window.GetSize(&width, &height);

        float size_x = (float)width*2.0f/3.0f;
        float size_y = (float)height - 168.0f;
        ImVec2 previewSize = ImVec2(size_x, size_y);

	    ImGui::SetNextWindowSize(previewSize);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;
	    ImGui::Begin("Preview", NULL, window_flags);
        const Texture* texture = renderer.GetTexture();
        float offset = 0.5f*(previewSize.y - previewSize.x * 10.0f / 16.0f);
        ImGui::SetCursorPos(ImVec2(0.0f, offset));
        ImGui::Image((void*)(intptr_t)texture->GetHandle(), 
            ImVec2(previewSize.x, previewSize.x* 10.0f / 16.0f),
		    ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        bool itemHovered = ImGui::IsItemHovered() && ImGui::IsWindowHovered();
        
        ImVec2 rectSize = ImGui::GetItemRectSize();
        if (itemHovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 windowSize = ImGui::GetWindowSize();
            
            ImVec2 borderSize = ImVec2(0.5f*(windowSize.x - rectSize.x), 0.5f*(windowSize.y - rectSize.y));
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImVec2 imageCoords = ImVec2(mousePos.x + (int)borderSize.x - 4, mousePos.y - 18 - (int)borderSize.y);

            renderer.SetSunPosition(glm::vec2(imageCoords.x, imageCoords.y), glm::vec2(rectSize.x, rectSize.y));
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            m_App->GetWindow().HideCursor();
            m_UpdateCamera = true;
        }

        if (m_UpdateCamera && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            m_App->GetWindow().ShowCursor();
            m_Camera->Reset();
            m_UpdateCamera = false;
        }

	    ImGui::End();
    }

    void Gui::Log() {
        auto& log = m_App->GetLog();
        auto& window = m_App->GetWindow();

        s32 width, height;
        window.GetSize(&width, &height);
        ImVec2 previewSize = ImVec2((float)width*2/3, 150.0f);

	    ImGui::SetNextWindowSize(previewSize);
        ImGui::SetNextWindowPos(ImVec2(0, height - 150.0f));

        ImGui::Begin("Log", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::TextUnformatted(log.buffer.begin());
        if (log.scroll_to_bottom)
            ImGui::SetScrollHere(1.0f);
        log.scroll_to_bottom = false;
        ImGui::End();
    }

    void Gui::Settings() {
        auto& window = m_App->GetWindow();
        auto& renderer = m_App->GetRenderer();

        s32 width, height;
        window.GetSize(&width, &height);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize;
        ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 18));
        ImGui::SetNextWindowSize(ImVec2((float)width*1/3, (float)height - 18.0f));
        ImGui::Begin("Settings", NULL, window_flags);

        if (ImGui::CollapsingHeader("Sun")) {
            glm::vec3 sun_dir = renderer.GetSunDirection();
            if (ImGui::DragFloat3("Sun direction", glm::value_ptr(sun_dir), 0.05f, -1.0f, 1.0f)) {
                if (sun_dir.y < -0.1f) {
                    sun_dir.y = 0.0f;
                }
                renderer.SetSunDirection(glm::normalize(sun_dir));
            }

            glm::vec3 sun_color = renderer.GetSunColor();
            ImGui::ColorEdit3("Sun Color", glm::value_ptr(sun_color), ImGuiColorEditFlags_Float);
            renderer.SetSunColor(sun_color);

            f32 sun_intensity = renderer.GetSunIntensity();
            if (ImGui::DragFloat("Sun intensity", &sun_intensity, 0.05f, 0.0f, 3.0f)) {
                sun_intensity = glm::clamp(sun_intensity, 0.0f, 3.0f);
                renderer.SetSunIntensity(sun_intensity);
            }

            f32 sun_size = renderer.GetSunSize();
            if (ImGui::DragFloat("Sun size", &sun_size, 0.05f, 1.0f, 10.0f)) {
                sun_size = glm::clamp(sun_size, 1.0f, 10.0f);
                renderer.SetSunSize(sun_size);
            }
        }

        if (ImGui::CollapsingHeader("FBM")) {
            s32 num_octaves = renderer.GetFbmOctaves();
            if (ImGui::DragInt("Number of octaves", &num_octaves, 1.0f, 2, 24)) {
                num_octaves = glm::clamp(num_octaves, 2, 24);
                renderer.SetFbmOctaves(num_octaves);
            }

            f32 fbm_frequency = renderer.GetFbmFrequency();
            if (ImGui::DragFloat("Frequency", &fbm_frequency, 0.001f, 0.001f, 2.0f)) {
                fbm_frequency = glm::clamp(fbm_frequency, 0.001f, 2.0f);
                renderer.SetFbmFrequency(fbm_frequency);
            }

            f32 fbm_amplitude = renderer.GetFbmAmplitude();
            if (ImGui::DragFloat("Amplitude", &fbm_amplitude, 0.01f, 0.0f, 2.0f)) {
                fbm_amplitude = glm::clamp(fbm_amplitude, 0.0f, 2.0f);
                renderer.SetFbmAmplitude(fbm_amplitude);
            }

            bool use_derivatives = renderer.GetFbmUseDerivatives();
            if (ImGui::Checkbox("Use derivatives", &use_derivatives)) {
                renderer.SetFbmUseDerivatives(use_derivatives);
            }

            if (use_derivatives) {
                f32 smoothness = renderer.GetFbmSmoothness();
                if (ImGui::DragFloat("Smoothness", &smoothness, 0.01f, 0.0f, 5.0f)) {
                    smoothness = glm::clamp(smoothness, 0.0f, 5.0f);
                    renderer.SetFbmSmoothness(smoothness);
                }

                const char* items[] = { "Quintic", "Cubic"};
                int item_current = static_cast<int>(renderer.GetFbmInterpolation());
                if (ImGui::Combo("Interpolation polynom", &item_current, items, IM_ARRAYSIZE(items))) {
                    renderer.SetFbmInterpolation(static_cast<FbmInterpolation>(item_current));
                }
            }
        }

        if (ImGui::CollapsingHeader("Terrain")) {
            f32 terrain_height = renderer.GetTerrainHeight();
            f32 snow_level = renderer.GetSnowLevel();
            f32 grass_level = renderer.GetGrassLevel();

            if (ImGui::DragFloat("Terrain Height", &terrain_height, 0.01f, 0.0f, 10.0f)) {
                terrain_height = glm::clamp(terrain_height, 0.0f, 10.0f);
                snow_level = glm::clamp(snow_level, 0.0f, terrain_height);
                grass_level = glm::clamp(grass_level, 0.0f, snow_level);
            }

            if (ImGui::DragFloat("Snow Level", &snow_level, 0.01f, 0.0f, terrain_height)) {
                snow_level = glm::clamp(snow_level, 0.0f, terrain_height);
                grass_level = glm::clamp(grass_level, 0.0f, snow_level);
            }

            
            if (ImGui::DragFloat("Grass Level", &grass_level, 0.01f, 0.0f, snow_level)) {
                grass_level = glm::clamp(grass_level, 0.0f, snow_level);
            }

            renderer.SetGrassLevel(grass_level);
            renderer.SetSnowLevel(snow_level);
            renderer.SetTerrainHeight(terrain_height);

            bool water_enabled = renderer.GetWaterEnabled();
            if (ImGui::Checkbox("Enable Water", &water_enabled)) {
                renderer.SetWaterEnabled(water_enabled);
            }

            if (water_enabled) {
                f32 water_level = renderer.GetWaterLevel();
                if (ImGui::DragFloat("Water Level", &water_level, 0.001f, 0.0f, terrain_height)) {
                    renderer.SetWaterLevel(water_level);
                }
            }
        }

        ImGui::End();
    }

    void Gui::RenderSettings() {
        if (m_RenderSettingsVisible) {
            auto& renderer = m_App->GetRenderer();

            ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f));
            ImGui::Begin("Render image", &m_RenderSettingsVisible);
            ImGui::PushItemWidth(100.0f);
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("Output: %s", (output_path + output_name).c_str());
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - 105.0f);
            if (ImGui::Button("Configure", ImVec2(100, 20))) {
                m_FileExplorerRender.Open();
            }
            ImGui::Separator();
            
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Width:");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - 105.0f);
            ImGui::DragInt("##width", &output_width, 1, 640, 8192);

            //ImGui::AlignTextToFramePadding();
            ImGui::Text("Height:");
            
            ImGui::AlignTextToFramePadding();
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - 105.0f);

            ImGui::DragInt("##height", &output_height, 1, 640, 8192);

            if (output_extension == ImageExtension::Jpeg) {
                ImGui::Separator();
                //ImGui::AlignTextToFramePadding();
                ImGui::Text("Quality:");
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - 105.0f);
                ImGui::DragInt("##quality", &output_quality, 1, 1, 100);
            }

            ImGui::Separator();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - 105.0f);
            if (ImGui::Button("Render", ImVec2(100, 20))) {
                renderer.RenderImage((output_path + output_name).c_str(), output_width, output_height, output_extension, output_quality);
                m_RenderSettingsVisible = false;
            }

            ImGui::PopItemWidth();
            ImGui::End();
        }
    }

    void Gui::RenderImage() {
        
        m_FileExplorerRender.Display();
        if(m_FileExplorerRender.HasSelected()) {
            output_path = m_FileExplorerRender.GetSelected().parent_path().u8string() + "\\";
            output_name = m_FileExplorerRender.GetSelected().filename().u8string();

            if (m_FileExplorerRender.GetSelected().extension().u8string() == ".png") {
                output_extension = ImageExtension::Png;
            }
            else if (m_FileExplorerRender.GetSelected().extension().u8string() == ".jpg" ||
                     m_FileExplorerRender.GetSelected().extension().u8string() == ".jpeg") {
                output_extension = ImageExtension::Jpeg;
            }
            else if (m_FileExplorerRender.GetSelected().extension().u8string() == ".bmp") {
                output_extension = ImageExtension::Bmp;
            }
            else {
                auto& log = m_App->GetLog();
                log.AddLog("Wrong extension: %s, using .png instead\n", m_FileExplorerRender.GetSelected().extension().u8string().c_str());
                output_name = m_FileExplorerRender.GetSelected().stem().u8string() + ".png";
                output_extension = ImageExtension::Png;
            }
            m_FileExplorerRender.ClearSelected();
            m_FileExplorerRender.Close();
        }
    }

     void Gui::Draw() {
        auto& io = ImGui::GetIO(); (void)io;
        if (m_UpdateCamera)
            m_Camera->Update(io.DeltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if ((Input::GetKey(KeyCode::LeftCtrl) || Input::GetKey(KeyCode::RightCtrl)) && Input::GetKey(KeyCode::O)) {
            if (!m_FileExplorerLoadConfig.IsOpened()) {
                m_FileExplorerLoadConfig.Open();
            }
        }

        if ((Input::GetKey(KeyCode::LeftCtrl) || Input::GetKey(KeyCode::RightCtrl)) && Input::GetKey(KeyCode::S)) {
            if (!m_FileExplorerSaveConfig.IsOpened()) {
                m_FileExplorerSaveConfig.Open();
            }
        }

        if (Input::GetKeyDown(KeyCode::F5)) {
            if (!m_RenderSettingsVisible) {
                m_RenderSettingsVisible = true;
                auto& window = m_App->GetWindow();
                s32 width;
                s32 height;
                window.GetSize(&width, &height);
                ImGui::SetNextWindowPos(ImVec2(width*0.5f, height*0.5f));
            }
        }
        
        MenuBar();
        Preview();
        Log();
        //Stats();
        Settings();

        SaveConfig();
        LoadConfig();
        RenderSettings();
        RenderImage();
        /* Shortcuts */
        
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    Gui::~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Gui::SaveConfig() {
        auto& renderer = m_App->GetRenderer();
        auto& log = m_App->GetLog();
        m_FileExplorerSaveConfig.Display();

        if(m_FileExplorerSaveConfig.HasSelected()) {
            std::string path =  m_FileExplorerSaveConfig.GetSelected().u8string().c_str();
            if (!m_FileExplorerSaveConfig.GetSelected().has_extension()) {
                path += ".json";
            }
            else if (!(m_FileExplorerSaveConfig.GetSelected().extension().u8string() == ".json")) {
                log.AddLog("Wrong %s extension: added .json automatically\n", path.c_str());
                path += ".json";
            }
            renderer.SaveConfig(path.c_str());
            m_FileExplorerSaveConfig.ClearSelected();
            m_FileExplorerSaveConfig.Close();
        }
    }

    void Gui::LoadConfig() {
        auto& renderer = m_App->GetRenderer();
        m_FileExplorerLoadConfig.Display();

        if(m_FileExplorerLoadConfig.HasSelected()) {
            renderer.LoadConfig( m_FileExplorerLoadConfig.GetSelected().u8string().c_str());
            m_FileExplorerLoadConfig.ClearSelected();
            m_FileExplorerLoadConfig.Close();
        }
    }   
}