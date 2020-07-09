#pragma once

#include <chrono>

#include "types.h"

namespace fbmgen {
    class Timer {
    public:
        void Run();
        void Stop();
        void Reset();

        u32 GetTimeMS() const;
        f64 GetTimeS() const;
    private:
        std::chrono::high_resolution_clock::time_point m_StartTime;
        std::chrono::high_resolution_clock::time_point m_EndTime;
        bool m_Stopped = true;
    };
}