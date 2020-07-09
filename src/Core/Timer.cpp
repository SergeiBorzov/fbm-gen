#include "Timer.h"

namespace fbmgen {
    void Timer::Run() {
        if (m_Stopped) {
            m_StartTime = std::chrono::high_resolution_clock::now();
            m_Stopped = false;
        }
    }

    void Timer::Stop() {
        if (!m_Stopped) {
            m_EndTime = std::chrono::high_resolution_clock::now();
            m_Stopped = true;
        }
    }

    void Timer::Reset() {
        Stop();
        Run();
    }

    f64 Timer::GetTimeS() const {
        if (!m_Stopped) {
            auto time_stamp = std::chrono::high_resolution_clock::now();
            return 0.001*std::chrono::duration_cast<std::chrono::milliseconds>(time_stamp - m_StartTime).count();
        }
        else {
            return 0.001*std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count();
        }
    }

    u32 Timer::GetTimeMS() const {
        if (!m_Stopped) {
            auto time_stamp = std::chrono::high_resolution_clock::now();
            return (u32)std::chrono::duration_cast<std::chrono::milliseconds>(time_stamp - m_StartTime).count();
        }
        else {
            return (u32)std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count();
        }
    }
}