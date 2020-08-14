#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include <imfilebrowser.h>

#include "../Core/types.h"
#include "../Graphics/ImageExtension.h"

namespace fbmgen {

    class Application;
    class Camera;

    class Gui {
    public:
        inline glm::ivec2 GetPreviewSize() const { return m_PreviewSize; }
        bool Create(Application* app);
        inline void SetCamera(Camera* camera) { m_Camera = camera; }
        void Draw();

        ~Gui();
    private:
        void MenuBar();
        void Preview();
        void Log();
        void Stats();
        void Settings();
        
        void LoadConfig();
        void SaveConfig();

        void RenderSettings();
        void RenderImage();

        ImGui::FileBrowser m_FileExplorerLoadConfig; 
        ImGui::FileBrowser m_FileExplorerSaveConfig = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
        ImGui::FileBrowser m_FileExplorerRender = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);

        std::string output_path = std::filesystem::current_path().u8string() + "\\";
        std::string output_name = "image.png";
        ImageExtension output_extension = ImageExtension::Png;

        std::string load_config_path;
        std::string load_config_name;

        Application* m_App = nullptr;

        Camera* m_Camera = nullptr;
        bool m_UpdateCamera = false;
        
        bool m_RenderSettingsVisible = false;
        bool m_ExportImage = false;

        glm::ivec2 m_PreviewSize;
        s32 output_width = 1920;
        s32 output_height = 1080;
        s32 output_quality = 100;
    };
    
}