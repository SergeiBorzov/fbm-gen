
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../Core/types.h"
#include "../Core/Application.h"

#include <cstdio>
namespace fbmgen {
    bool Gui::Create(Application* app) {
        m_App = app;

        if (m_App) {
             IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            auto& io = ImGui::GetIO(); (void)io;
            // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(m_App->GetWindow().GetPtr(), true);
            ImGui_ImplOpenGL3_Init("#version 130");
            return true;
        }

        return false;       
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
                if (ImGui::MenuItem("Open", "Ctrl + O")) {
                    m_FileExplorer.SetTitle("Open config file");
                    m_FileExplorer.Open();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl + S")) {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt + F4")) {
                    window.Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Render Image", "F5")) {

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

        ImVec2 previewSize = ImVec2((float)width*2/3, (float) (height - 18));
	    ImGui::SetNextWindowSize(previewSize);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
	    ImGui::Begin("Preview", NULL, window_flags);


        const Texture* texture = renderer.GetTexture();
        float offset = 0.5f*(previewSize.y - previewSize.x * 10 / 16);

        ImGui::SetCursorPos(ImVec2(0.0f, offset));
        ImGui::Image((void*)(intptr_t)texture->GetHandle(), 
            ImVec2(previewSize.x, previewSize.x * 10 / 16),
		    ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	    ImGui::End();
    }

    void Gui::Stats() {
        auto& window = m_App->GetWindow();
        auto& renderer = m_App->GetRenderer();

        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
       
        s32 width, height;
        window.GetSize(&width, &height);

        ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 18));
        ImGui::Begin("Stats", NULL, window_flags);
            ImGui::Text("Platform: %s", renderer.GetPlatformName());
            ImGui::Text("Device: %s", renderer.GetDeviceName());
            ImGui::Text("FPS: ");
        ImGui::End();
    }

    void Gui::Draw() {
        auto& io = ImGui::GetIO(); (void)io;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        MenuBar();
        Preview();
        Stats();

        FileExplorer();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    Gui::~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Gui::FileExplorer() {
        m_FileExplorer.Display();
        
        if(m_FileExplorer.HasSelected())
        {
            fprintf(stderr, "%s\n", m_FileExplorer.GetSelected().string().c_str());
            m_FileExplorer.ClearSelected();
        }
    }   
}