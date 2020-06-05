#pragma once

#include <GL/glew.h>
#define GLEW_STATIC

#include <GLFW/glfw3.h>

namespace fbmgen {

    class Window {
    public:
        bool Create();
        void Update();
        bool ShouldClose() const { return glfwWindowShouldClose(m_WindowPointer); }
        ~Window();
    private:
        static void ErrorCallback(int error, const char* description);
        
        struct WindowData {
            const char* title = "fbm-gen";
            int width = 640;
            int height = 480;
        };

        WindowData m_WindowData;
        GLFWwindow* m_WindowPointer = nullptr;
        bool m_Inited = false;
    };

}