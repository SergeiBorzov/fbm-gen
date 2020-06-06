#pragma once

#include <GL/glew.h>
#define GLEW_STATIC

#include <GLFW/glfw3.h>

namespace fbmgen {

    class Window {
    friend class Application;
    public:
        void GetPosition(int* x, int* y) const { glfwGetWindowPos(m_WindowPointer, x, y); }
        void SetPosition(int x, int y) { glfwSetWindowPos(m_WindowPointer, x, y); }

        bool Create();
        void Update();
        bool ShouldClose() const { return glfwWindowShouldClose(m_WindowPointer); }
        ~Window();
    private:
        static void ErrorCallback(int error, const char* description);
        static void KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow* window, double x, double y);
        
        struct WindowData {
            const char* title = "fbm-gen";
            int width = 1280;
            int height = 720;
        };

        WindowData m_WindowData;
        GLFWwindow* m_WindowPointer = nullptr;
        bool m_Inited = false;
    };

}