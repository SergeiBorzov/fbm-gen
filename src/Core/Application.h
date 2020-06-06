#pragma once 

#include "../Window/Window.h"
#include "../Graphics/Renderer.h"
#include "../Gui/Gui.h"

namespace fbmgen {
    class Application {
    public:
        bool Create();
        void Run();
        ~Application();
    private:
        Window* m_Window = nullptr;
        Renderer* m_Renderer = nullptr;
        Gui* m_Gui = nullptr;
    };
}