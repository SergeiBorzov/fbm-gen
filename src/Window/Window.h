#pragma once

#include <GL/glew.h>
#define GLEW_STATIC

#include <GLFW/glfw3.h>

#include "../Core/types.h"

namespace fbmgen {

    class Window {
    friend class Application;
    public:
        void GetPosition(s32* x, s32* y) const { glfwGetWindowPos(m_WindowPointer, x, y); }
        void SetPosition(s32 x, s32 y) { glfwSetWindowPos(m_WindowPointer, x, y); }

        void SetSize(s32 width, s32 height) { m_WindowData.width = width; m_WindowData.height = height; }
        void GetSize(s32* width, s32* heigth) const { *width = m_WindowData.width; *heigth = m_WindowData.height; }

        bool Create();
        void Update();
        bool ShouldClose() const { return glfwWindowShouldClose(m_WindowPointer); }
        ~Window();
    private:
        static void ErrorCallback(s32 error, const char* description);
        static void KeyPressCallback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
        static void MouseButtonCallback(GLFWwindow* window, s32 button, s32 action, s32 mods);
        static void CursorPositionCallback(GLFWwindow* window, double x, double y);
        
        struct WindowData {
            const char* title = "fbm-gen";
            s32 width = 1280;
            s32 height = 720;
        };

        WindowData m_WindowData;
        GLFWwindow* m_WindowPointer = nullptr;
        bool m_Inited = false;
    };

}