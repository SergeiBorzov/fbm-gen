#pragma once

#include "../Core/types.h"

namespace fbmgen {

    enum KeyCode {
        Space        = 32,
        Apostrophe   = 39, 
        Comma        = 44, 
        Minus        = 45,
        Period       = 46, 
        Slash        = 47,
        Key_0        = 48, 
        Key_1        = 49, 
        Key_2        = 50, 
        Key_3        = 51, 
        Key_4        = 52, 
        Key_5        = 53, 
        Key_6        = 54, 
        Key_7        = 55, 
        Key_8        = 56, 
        Key_9        = 57, 
        Semicolon    = 59, 
        Equal        = 61, 
        A            = 65, 
        B            = 66, 
        C            = 67, 
        D            = 68, 
        E            = 69, 
        F            = 70, 
        G            = 71, 
        H            = 72, 
        I            = 73, 
        J            = 74, 
        K            = 75, 
        L            = 76, 
        M            = 77, 
        N            = 78, 
        O            = 79, 
        P            = 80, 
        Q            = 81, 
        R            = 82, 
        S            = 83, 
        T            = 84, 
        U            = 85, 
        V            = 86, 
        W            = 87, 
        X            = 88, 
        Y            = 89, 
        Z            = 90, 
        LeftBracket  = 91, 
        Backslash    = 92, 
        RightBracket = 93,
        GraveAccent  = 96, 
        World1       = 161,
        World2       = 162,
        Escape       = 256,
        Enter        = 257, 
        Tab          = 258, 
        Backspace    = 259, 
        Insert       = 260, 
        Delete       = 261, 
        Right        = 262, 
        Left         = 263, 
        Down         = 264, 
        Up           = 265, 
        PageUp       = 266, 
        PageDown     = 267, 
        Home         = 268, 
        END          = 269, 
        CapsLock     = 280, 
        ScrollLock   = 281, 
        NumLock      = 282, 
        PrintScreen  = 283, 
        Pause        = 284, 
        F1           = 290, 
        F2           = 291, 
        F3           = 292, 
        F4           = 293, 
        F5           = 294, 
        F6           = 295, 
        F7           = 296, 
        F8           = 297, 
        F9           = 298, 
        F10          = 299, 
        F11          = 300, 
        F12          = 301, 
        F13          = 302, 
        F14          = 303, 
        F15          = 304, 
        F16          = 305, 
        F17          = 306, 
        F18          = 307, 
        F19          = 308, 
        F20          = 309, 
        F21          = 310, 
        F22          = 311, 
        F23          = 312, 
        F24          = 313, 
        F25          = 314, 
        Kp0         = 320, 
        Kp1         = 321, 
        Kp2         = 322, 
        Kp3         = 323, 
        Kp4         = 324, 
        Kp5         = 325, 
        Kp6         = 326, 
        Kp7         = 327, 
        Kp8         = 328, 
        Kp9         = 329, 
        KpDecimal   = 330, 
        KpDivide    = 331, 
        KpMultiply  = 332, 
        KpSubtract  = 333, 
        KpAdd       = 334, 
        KpEnter     = 335, 
        KpEqual     = 336, 
        LeftShift   = 340, 
        LeftCtrl    = 341, 
        LeftAlt     = 342, 
        LeftCmd     = 343, 
        RightShift  = 344, 
        RightCtrl   = 345, 
        RightAlt    = 346, 
        RightCmd    = 347, 
        Menu        = 348 
    };

    enum MouseButton {
        ButtonLeft      = 0,
        ButtonRight     = 1,
        ButtonMiddle    = 2,
        Button4   = 3,
        Button5   = 4,
        Button6   = 5,
        Button7   = 6,
        Button8   = 7
    };




    class Input {
    friend class Window;
    public:
        static bool GetKey(KeyCode key) { return _Keys[key]; }
        static bool GetKeyDown(KeyCode key) { return !_KeysPrevious[key] && _Keys[key]; }
        static bool GetKeyUp(KeyCode key) { return _KeysPrevious[key] && !_Keys[key]; }

        static bool GetMouse(MouseButton code) { return _Mouse[code]; }
        static bool GetMouseDown(MouseButton code) { return !_MousePrevious[code] && _Mouse[code]; }
        static bool GetMouseUp(MouseButton code) { return _MousePrevious[code] && !_Mouse[code]; }

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