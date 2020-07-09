#include <cstdio>
#include <cmath>
#include <chrono>

#include "../Core/Timer.h"
#include "Application.h"
#include "../Input/Input.h"


namespace fbmgen {

    bool Application::Create() {

        m_Window = new Window();
        m_Window->SetSize(1536, 658);
        m_Renderer = new Renderer();
        m_Gui = new Gui();

        if (!m_Window->Create()) {
            fprintf(stderr, "Error: Can't create window.");
            return false;
        }

        if (!m_Renderer->Create(this)) {
            m_Log.AddLog("Error: Can't create renderer.");
            return false;
        }

        if (!m_Gui->Create(this)) {
            m_Log.AddLog("Error: Can't create gui.");
            return false;
        }
        return true;
    }

    void Application::Run() {
        s32 width, height;
        m_Window->GetSize(&width, &height);
        Camera* mainCamera = new Camera();
        m_Renderer->SetCamera(mainCamera);
        mainCamera->Start();

        Timer timer;
        timer.Run();
        while(!m_Window->ShouldClose()) {
            f32 dt = (f32)timer.GetTimeS();
            timer.Reset();
            Input::Update();
            m_Window->Update();
            mainCamera->Update(dt);
            m_Renderer->Draw();
            m_Gui->Draw();
        }
        delete mainCamera;
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