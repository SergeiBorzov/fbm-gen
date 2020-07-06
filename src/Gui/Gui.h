#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../Graphics/Renderer.h"

namespace fbmgen {

    class Gui {
    public:
        bool Create(GLFWwindow* windowPointer, Renderer* renderer);
        void Draw();

        ~Gui();
    private:
        void MenuBar();
        void Preview();
        void Stats();

        Renderer* m_Renderer = nullptr;
        bool m_StatsVisible = true;
    };
    
}