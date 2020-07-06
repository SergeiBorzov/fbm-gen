#include <cstring>

#include "Input.h"

namespace fbmgen {
    bool Input::_Keys[512] = {false};
    bool Input::_KeysPrevious[512] = {false};

    bool Input::_Mouse[8] = {false};
    bool Input::_MousePrevious[8] = {false};

    f64 Input::_Cursor_X = 0.0;
    f64 Input::_Cursor_Y = 0.0;
    
    void Input::Update() {
        memcpy(_KeysPrevious, _Keys, 512);
        memcpy(_MousePrevious, _Mouse, 8);
    }
}