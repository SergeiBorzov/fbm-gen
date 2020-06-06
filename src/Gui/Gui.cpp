
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Gui.h"

#include <cstdio>
namespace fbmgen {
    void Gui::Create(GLFWwindow* windowPointer) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
         // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(windowPointer, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    void Gui::DrawStats() {
        ImGui::SetNextWindowSize(ImVec2(160, 90));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoResize;
        if (m_StatsVisible) {
            ImGui::Begin("Stats", &m_StatsVisible, window_flags);
                ImGui::Text("Platform: ");
                ImGui::Text("Device: ");
                ImGui::Text("FPS: ");
               
            ImGui::End();
        }
    }

    void Gui::DrawMenuBar() {
        if(ImGui::BeginMainMenuBar()) {
            
            if (ImGui::BeginMenu("File")) {
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
                    fprintf(stderr, "Hello\n");
                }
                if (ImGui::MenuItem("New", "Ctrl + N")) {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Open", "Ctrl + O")) {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl + S")) {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt + F4")) {

                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Render", "F5")) {

                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void Gui::Draw() {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawMenuBar();
        DrawStats();


        ImGui::ShowDemoWindow();

      
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    Gui::~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}