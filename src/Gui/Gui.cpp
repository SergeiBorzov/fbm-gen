#include "Gui.h"

#include "../Third_Party/ImGui/imgui_impl_glfw.h"
#include "../Third_Party/ImGui/imgui_impl_opengl3.h"

namespace fbmgen {
    bool Gui::Create() {
        // Get window from context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        //ImGui_ImplGlfw_InitForOpenGL(window, true);
        //ImGui_ImplOpenGL3_Init(glsl_version);
        return true;
    }

    void Gui::Draw() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}