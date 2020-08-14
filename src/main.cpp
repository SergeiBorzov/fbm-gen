#include <Windows.h>
#include <memory>
#include "Core/Application.h"


int WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, int){
    std::unique_ptr<fbmgen::Application> app = std::make_unique<fbmgen::Application>();

    if (!app->Create()) {
        exit(EXIT_FAILURE);
    }

    app->Run();
    return 0;
}
