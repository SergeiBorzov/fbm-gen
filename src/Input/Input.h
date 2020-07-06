#pragma once

#include "../Core/types.h"

namespace fbmgen {
    class Input {
    friend class Window;
    public:
        static bool GetKey(int key) { return _Keys[key]; }
        static bool GetKeyDown(int key) { return !_KeysPrevious[key] && _Keys[key]; }
        static bool GetKeyUp(int key) { return _KeysPrevious[key] && !_Keys[key]; }

        static bool GetMouse(int code) { return _Mouse[code]; }
        static bool GetMouseDown(int code) { return !_MousePrevious[code] && _Mouse[code]; }
        static bool GetMouseUp(int code) { return _MousePrevious[code] && !_Mouse[code]; }

        static f64 GetCursorX() { return _Cursor_X; }
        static f64 GetCursorY() { return _Cursor_Y; }

        static void Update();
    private:
        static bool _KeysPrevious[512];
        static bool _Keys[512];

        static bool _MousePrevious[8];
        static bool _Mouse[8];

        static f64 _Cursor_X;
        static f64 _Cursor_Y;
        
    };
}