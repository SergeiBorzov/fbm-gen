#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace fbmgen {

    class Gui {
    public:
        void Create(GLFWwindow* windowPointer);
        void Draw();

        ~Gui();
    private:
        void DrawMenuBar();
        void DrawStats();

        bool m_StatsVisible = true;
    };
    
}