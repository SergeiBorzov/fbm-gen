#include <cstdio>
#include <cmath>

#include "Application.h"
#include "../Input/Input.h"


namespace fbmgen {

    bool Application::Create() {
        m_Window = new Window();
        m_Window->SetSize(1366, 768);
        m_Renderer = new Renderer();
        m_Gui = new Gui();

        if (!m_Window->Create()) {
            fprintf(stderr, "Error: Can't create window.");
            return false;
        }

        if (!m_Renderer->Create()) {
            fprintf(stderr, "Error: Can't create renderer.");
            return false;
        }

        m_Gui->Create(m_Window->m_WindowPointer, m_Renderer);

        return true;
    }

    void Application::Run() {       
        while(!m_Window->ShouldClose()) {
            Input::Update();
            m_Window->Update();
            m_Renderer->Draw();
            m_Gui->Draw();
        }
    }

    Application::~Application() {
        if (m_Gui) {
            delete m_Gui;
        }
        if (m_Renderer) {
            delete m_Renderer;
        }
        if (m_Window) {
            delete m_Window;
        }
    }
}