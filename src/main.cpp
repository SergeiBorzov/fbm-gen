#include <cstdlib>
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Gui/Gui.h"

int main() {
    fbmgen::Window* window = new fbmgen::Window();
    fbmgen::Renderer* renderer = new fbmgen::Renderer();


    if (!window->Create()) {
        exit(EXIT_FAILURE);
    }

    while(!window->ShouldClose()) {
        window->Update();
        renderer->Draw();
    }

    delete renderer;
    delete window;

    return 0;
}
