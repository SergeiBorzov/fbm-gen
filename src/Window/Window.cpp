
#include <cstdio>

#include "Window.h"
#include "../Input/Input.h"

namespace fbmgen {

    void Window::ErrorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }


    void Window::MouseButtonCallback(GLFWwindow*, int button, int action, int) {
        if (action == GLFW_PRESS) {
            Input::_Mouse[button] = true;
        }
        else if (action == GLFW_RELEASE) {
            Input::_Mouse[button] = false;
        }
    }

    void Window::KeyPressCallback(GLFWwindow*, int key, int, int action, int) {
        if (action == GLFW_PRESS) {
            Input::_Keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            Input::_Keys[key] = false;
        }
    }

    void Window::CursorPositionCallback(GLFWwindow*, double x, double y) {
        Input::_Cursor_X = x;
        Input::_Cursor_Y = y;
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
        //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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

        glfwSetKeyCallback(m_WindowPointer, KeyPressCallback);
        glfwSetMouseButtonCallback(m_WindowPointer, MouseButtonCallback);
        glfwSetCursorPosCallback(m_WindowPointer, CursorPositionCallback);

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
        glfwWaitEvents();
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