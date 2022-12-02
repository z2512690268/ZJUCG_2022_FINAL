#ifndef _CALLBACK_H
#define _CALLBACK_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

enum CALLBACK_KEY
{
    CALLBACK_KEY_SPACE =             32,
    CALLBACK_KEY_APOSTROPHE =        39,
    CALLBACK_KEY_PLUS =              43,
    CALLBACK_KEY_COMMA =             44,
    CALLBACK_KEY_MINUS =             45,
    CALLBACK_KEY_PERIOD =            46,
    CALLBACK_KEY_SLASH =             47,
    CALLBACK_KEY_0 =                 48,
    CALLBACK_KEY_1 =                 49,
    CALLBACK_KEY_2 =                 50,
    CALLBACK_KEY_3 =                 51,
    CALLBACK_KEY_4 =                 52,
    CALLBACK_KEY_5 =                 53,
    CALLBACK_KEY_6 =                 54,
    CALLBACK_KEY_7 =                 55,
    CALLBACK_KEY_8 =                 56,
    CALLBACK_KEY_9 =                 57,
    CALLBACK_KEY_SEMICOLON =         58,
    CALLBACK_KEY_EQUAL =             61,
    CALLBACK_KEY_A =                 65,
    CALLBACK_KEY_B =                 66,
    CALLBACK_KEY_C =                 67,
    CALLBACK_KEY_D =                 68,
    CALLBACK_KEY_E =                 69,
    CALLBACK_KEY_F =                 70,
    CALLBACK_KEY_G =                 71,
    CALLBACK_KEY_H =                 72,
    CALLBACK_KEY_I =                 73,
    CALLBACK_KEY_J =                 74,
    CALLBACK_KEY_K =                 75,
    CALLBACK_KEY_L =                 76,
    CALLBACK_KEY_M =                 77,
    CALLBACK_KEY_N =                 78,
    CALLBACK_KEY_O =                 79,
    CALLBACK_KEY_P =                 80,
    CALLBACK_KEY_Q =                 81,
    CALLBACK_KEY_R =                 82,
    CALLBACK_KEY_S =                 83,
    CALLBACK_KEY_T =                 84,
    CALLBACK_KEY_U =                 85,
    CALLBACK_KEY_V =                 86,
    CALLBACK_KEY_W =                 87,
    CALLBACK_KEY_X =                 88,
    CALLBACK_KEY_Y =                 89,
    CALLBACK_KEY_Z =                 90,
    CALLBACK_KEY_LEFT_BRACKET =      91,
    CALLBACK_KEY_BACKSLASH =         92,
    CALLBACK_KEY_RIGHT_BRACKET =     93,
    CALLBACK_KEY_a =                 97,
    CALLBACK_KEY_b =                 98,
    CALLBACK_KEY_c =                 99,
    CALLBACK_KEY_d =                 100,
    CALLBACK_KEY_e =                 101,
    CALLBACK_KEY_f =                 102,
    CALLBACK_KEY_g =                 103,
    CALLBACK_KEY_h =                 104,
    CALLBACK_KEY_i =                 105,
    CALLBACK_KEY_j =                 106,
    CALLBACK_KEY_k =                 107,
    CALLBACK_KEY_l =                 108,
    CALLBACK_KEY_m =                 109,
    CALLBACK_KEY_n =                 110,
    CALLBACK_KEY_o =                 111,
    CALLBACK_KEY_p =                 112,
    CALLBACK_KEY_q =                 113,
    CALLBACK_KEY_r =                 114,
    CALLBACK_KEY_s =                 115,
    CALLBACK_KEY_t =                 116,
    CALLBACK_KEY_u =                 117,
    CALLBACK_KEY_v =                 118,
    CALLBACK_KEY_w =                 119,
    CALLBACK_KEY_x =                 120,
    CALLBACK_KEY_y =                 121,
    CALLBACK_KEY_z =                 122,
    CALLBACK_KEY_ESCAPE,
    CALLBACK_KEY_ENTER,          
    CALLBACK_KEY_TAB,            
    CALLBACK_KEY_BACKSPACE,      
    CALLBACK_KEY_INSERT,         
    CALLBACK_KEY_DELETE,            
    CALLBACK_KEY_RIGHT,             
    CALLBACK_KEY_LEFT,               
    CALLBACK_KEY_DOWN,               
    CALLBACK_KEY_UP,         
    CALLBACK_KEY_PAGE_UP,   
    CALLBACK_KEY_PAGE_DOWN,      
    CALLBACK_KEY_HOME,    
    CALLBACK_KEY_END,     
    CALLBACK_KEY_F1,        
    CALLBACK_KEY_F2,          
    CALLBACK_KEY_F3,       
    CALLBACK_KEY_F4,   
    CALLBACK_KEY_F5,      
    CALLBACK_KEY_F6,     
    CALLBACK_KEY_F7,     
    CALLBACK_KEY_F8,     
    CALLBACK_KEY_F9,    
    CALLBACK_KEY_F10,    
    CALLBACK_KEY_F11,    
    CALLBACK_KEY_F12,
    CALLBACK_KEY_UNDEFINED = 999,
};

enum CALLBACK_KEY_STATE {
    CALLBACK_KEY_STATE_RELEASE = 0,
    CALLBACK_KEY_STATE_PRESS
};

enum CALLBACK_MOUSE {
    CALLBACK_MOUSE_BUTTON_LEFT,
    CALLBACK_MOUSE_BUTTON_MIDDLE,
    CALLBACK_MOUSE_BUTTON_RIGHT,
    CALLBACK_MOUSE_UNDEFINED = 999
};

enum CALLBACK_MOUSE_STATE {
    CALLBACK_MOUSE_STATE_RELEASE = 0,
    CALLBACK_MOUSE_STATE_PRESS
};

class ICallbacks
{
private:
    CALLBACK_KEY_STATE key_states[256];
    CALLBACK_MOUSE_STATE mouse_states[3];
private:
public:
    CALLBACK_KEY_STATE GetKeyState(CALLBACK_KEY Key) {
        return key_states[Key];
    }

    CALLBACK_MOUSE_STATE GetMouseState(CALLBACK_MOUSE Button) {
        return mouse_states[Button];
    }

    CALLBACK_KEY_STATE SetKeyState(CALLBACK_KEY Key, CALLBACK_KEY_STATE State) {
        return key_states[Key] = State;
    }

    CALLBACK_MOUSE_STATE SetMouseState(CALLBACK_MOUSE Button, CALLBACK_MOUSE_STATE State) {
        return mouse_states[Button] = State;
    }

    virtual void KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y) {};
    
    virtual void PassiveMouseCB(int x, int y) {};

    virtual void MouseCB(CALLBACK_MOUSE Button, CALLBACK_MOUSE_STATE State, int x, int y) {};

    virtual void MouseMotionCB(int x, int y) {};

    virtual void RenderSceneCB() {};
};

CALLBACK_KEY GLUTSpecialKeyToCBKey(int Key);
CALLBACK_KEY GLUTKeyToCBKey(int Key);
CALLBACK_MOUSE GLUTMouseToCBMouse(int Button);

void InitCallbacks();
void DeleteCallbacks();

void RegisterICallback(ICallbacks* callback);
void UnregisterICallback();

#endif