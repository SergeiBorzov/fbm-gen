#include <cstdio>

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

        ImGui::StyleColorsDark();
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
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {

                }
                if (ImGui::MenuItem("New", "Ctrl + N")) {

                }
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

    void Gui::Stats() {
        auto& window = m_App->GetWindow();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
       
        s32 width, height;
        window.GetSize(&width, &height);

        ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 18));
        ImGui::SetNextWindowSize(ImVec2(150, 80));
        ImGui::Begin("Stats", NULL, window_flags);
            ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void Gui::Sky() {
        auto& window = m_App->GetWindow();
        auto& renderer = m_App->GetRenderer();

        s32 width, height;
        window.GetSize(&width, &height);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 98));
        ImGui::Begin("Sky", NULL, window_flags);

        glm::vec3 dir = renderer.GetSunDirection();
        float sundir[3] = { dir.x, dir.y, dir.z }; 
        if (ImGui::DragFloat3("Sun direction", sundir, 0.05f, -1.0f, 1.0f)) {
            glm::vec3 result = {sundir[0], sundir[1], sundir[2]};
            result = glm::normalize(result);
            renderer.SetSunDirection(result);
        }

        glm::vec3 sun_color = renderer.GetSunColor();
        float suncolor[3] = {sun_color.x, sun_color.y, sun_color.z};
        ImGui::ColorEdit3("Sun Color", suncolor, ImGuiColorEditFlags_Float);
        renderer.SetSunColor(glm::vec3(suncolor[0], suncolor[1], suncolor[2]));
        ImGui::End();

    }

   

    void Gui::SaveConfig() {
        m_FileExplorerSaveConfig.Display();
        
        if(m_FileExplorerSaveConfig.HasSelected())
        {
            fprintf(stderr, "%s\n", m_FileExplorerSaveConfig.GetSelected().string().c_str());
            m_FileExplorerSaveConfig.ClearSelected();
            m_FileExplorerSaveConfig.Close();
        }
    }

    void Gui::RenderSettings() {
        if (m_RenderSettingsVisible) {
            auto& renderer = m_App->GetRenderer();

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
        Stats();
        Sky();

        SaveConfig();
        LoadConfig();
        RenderSettings();
        RenderImage();

        
        ImGui::ShowDemoWindow();

        /* Shortcuts */
        
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    Gui::~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Gui::LoadConfig() {
        m_FileExplorerLoadConfig.Display();
        
        if(m_FileExplorerLoadConfig.HasSelected())
        {
            fprintf(stderr, "%s\n", m_FileExplorerLoadConfig.GetSelected().string().c_str());
            m_FileExplorerLoadConfig.ClearSelected();
            m_FileExplorerLoadConfig.Close();
        }
    }   
}