#include <cstdlib>
#include "Core/Application.h"

int main() {
    fbmgen::Application app;

    if (!app.Create()) {
        exit(EXIT_FAILURE);
    }

    app.Run();
    return 0;
}
