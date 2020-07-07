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
        void FileExplorer();

        ImGui::FileBrowser m_FileExplorer;
        Application* m_App = nullptr;
        
    };
    
}