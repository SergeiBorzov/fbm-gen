#include <cstdarg>
#include "Log.h"

namespace fbmgen {
    void Log::Clear() {
        buffer.clear();
    }

    void Log::AddLog(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        buffer.appendfv(fmt, args);
        va_end(args);
        scroll_to_bottom = true;
    }
}