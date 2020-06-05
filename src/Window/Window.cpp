
#include <cstdio>

#include "Window.h"

namespace fbmgen {

    void Window::ErrorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW error: %s\n", description);
        return;
    }

    bool Window::Create() {

        /* GLFW initialization */
        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

        // Mac OS support
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

        m_WindowPointer = glfwCreateWindow(
                            m_WindowData.width, 
                            m_WindowData.height,
                            m_WindowData.title,
                            NULL,
                            NULL);

        if (!m_WindowPointer) {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_WindowPointer);

        // Glew initialization
        glewExperimental = true; 
        if (glewInit() != GLEW_OK) { 
            glfwDestroyWindow(m_WindowPointer);
            glfwTerminate();
            return false;
        }

        glfwSwapInterval(0);

        m_Inited = true;
        return true;
    }

    void Window::Update() {
        glfwSwapBuffers(m_WindowPointer);
        glfwPollEvents();
    }

    Window::~Window() {
        if (m_WindowPointer) {
            glfwDestroyWindow(m_WindowPointer);
        }

        if (m_Inited) {
            glfwTerminate();
        }

    }

}