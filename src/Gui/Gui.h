#pragma once


#include <imgui.h>
#include <imfilebrowser.h>


namespace fbmgen {

    class Application;

    class Gui {
    public:
        bool Create(Application* app);
        void Draw();

        ~Gui();
    private:
        void MenuBar();
        void Preview();
        void Stats();
        
        void LoadConfig();
        void SaveConfig();
        void RenderImage();

        ImGui::FileBrowser m_FileExplorerLoadConfig;
        ImGui::FileBrowser m_FileExplorerSaveConfig;
        ImGui::FileBrowser m_FileExplorerRender;

        Application* m_App = nullptr;
    };
    
}