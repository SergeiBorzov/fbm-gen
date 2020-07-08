#pragma once 

#include "../Window/Window.h"
#include "../Graphics/Renderer.h"
#include "../Gui/Gui.h"
#include "../Gui/Log.h"

namespace fbmgen {
    class Application {
    public:
        bool Create();
        void Run();

        inline Renderer& GetRenderer() const { return *m_Renderer; }
        inline Window& GetWindow() const { return *m_Window; }
        inline Gui& GetGui() const { return *m_Gui; }
        inline Log& GetLog() { return m_Log; }

        ~Application();
    private:
        Window* m_Window = nullptr;
        Renderer* m_Renderer = nullptr;
        Gui* m_Gui = nullptr;
        Log m_Log;
    };
}