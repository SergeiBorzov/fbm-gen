#pragma once

#include "Gui.h"

namespace fbmgen {
    struct Log {
        ImGuiTextBuffer buffer;
        bool scroll_to_bottom;

        void Clear();
        void AddLog(const char* fmt, ...);
    };
}