#include <imgui.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl2.h>

#include "callback.h"

static ICallbacks dummy_callbacks;
static ICallbacks *g_pCallbacks = NULL;
//特殊按键控制
CALLBACK_KEY GLUTSpecialKeyToCBKey(int Key)
{
    switch (Key)
    {
    case GLUT_KEY_F1:
        return CALLBACK_KEY_F1;
    case GLUT_KEY_F2:
        return CALLBACK_KEY_F2;
    case GLUT_KEY_F3:
        return CALLBACK_KEY_F3;
    case GLUT_KEY_F4:
        return CALLBACK_KEY_F4;
    case GLUT_KEY_F5:
        return CALLBACK_KEY_F5;
    case GLUT_KEY_F6:
        return CALLBACK_KEY_F6;
    case GLUT_KEY_F7:
        return CALLBACK_KEY_F7;
    case GLUT_KEY_F8:
        return CALLBACK_KEY_F8;
    case GLUT_KEY_F9:
        return CALLBACK_KEY_F9;
    case GLUT_KEY_F10:
        return CALLBACK_KEY_F10;
    case GLUT_KEY_F11:
        return CALLBACK_KEY_F11;
    case GLUT_KEY_F12:
        return CALLBACK_KEY_F12;
    case GLUT_KEY_LEFT:
        return CALLBACK_KEY_LEFT;
    case GLUT_KEY_UP:
        return CALLBACK_KEY_UP;
    case GLUT_KEY_RIGHT:
        return CALLBACK_KEY_RIGHT;
    case GLUT_KEY_DOWN:
        return CALLBACK_KEY_DOWN;
    case GLUT_KEY_PAGE_UP:
        return CALLBACK_KEY_PAGE_UP;
    case GLUT_KEY_PAGE_DOWN:
        return CALLBACK_KEY_PAGE_DOWN;
    case GLUT_KEY_HOME:
        return CALLBACK_KEY_HOME;
    case GLUT_KEY_END:
        return CALLBACK_KEY_END;
    case GLUT_KEY_INSERT:
        return CALLBACK_KEY_INSERT;
    case GLUT_KEY_DELETE:
        return CALLBACK_KEY_DELETE;
    default:
        return CALLBACK_KEY_UNDEFINED;
    }
}
//按键回调
CALLBACK_KEY GLUTKeyToCBKey(int Key)
{
    if (
        ((Key >= '+') && (Key <= '9')) ||
        ((Key >= 'A') && (Key <= 'Z')) ||
        ((Key >= 'a') && (Key <= 'z')))
    {
        return (CALLBACK_KEY)Key; 
    }
    else
    {
        return CALLBACK_KEY_UNDEFINED;
    }
}
//鼠标回调
CALLBACK_MOUSE GLUTMouseToCBMouse(int Button)
{
    switch (Button)
    {
    case GLUT_LEFT_BUTTON:
        return CALLBACK_MOUSE_BUTTON_LEFT;
    case GLUT_MIDDLE_BUTTON:
        return CALLBACK_MOUSE_BUTTON_MIDDLE;
    case GLUT_RIGHT_BUTTON:
        return CALLBACK_MOUSE_BUTTON_RIGHT;
    default:
        return CALLBACK_MOUSE_UNDEFINED;
    }
}
//各类回调接口
static void _KeyboardCB(unsigned char key, int x, int y)
{
    CALLBACK_KEY cb_key = GLUTKeyToCBKey(key);
    g_pCallbacks->SetKeyState(cb_key, CALLBACK_KEY_STATE_PRESS);
    g_pCallbacks->KeyboardCB(cb_key, CALLBACK_KEY_STATE_PRESS, x, y);
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

static void _KeyboardUpCB(unsigned char key, int x, int y)
{
    CALLBACK_KEY cb_key = GLUTKeyToCBKey(key);
    g_pCallbacks->SetKeyState(cb_key, CALLBACK_KEY_STATE_RELEASE);
    g_pCallbacks->KeyboardCB(cb_key, CALLBACK_KEY_STATE_RELEASE, x, y);
    ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
}
static void _SpecialKeyboardCB(int key, int x, int y)
{
    CALLBACK_KEY cb_key = GLUTSpecialKeyToCBKey(key);
    g_pCallbacks->SetKeyState(cb_key, CALLBACK_KEY_STATE_PRESS);
    g_pCallbacks->KeyboardCB(cb_key, CALLBACK_KEY_STATE_PRESS, x, y);
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
}
static void _SpecialKeyboardUpCB(int key, int x, int y)
{
    CALLBACK_KEY cb_key = GLUTSpecialKeyToCBKey(key);
    g_pCallbacks->SetKeyState(cb_key, CALLBACK_KEY_STATE_RELEASE);
    g_pCallbacks->KeyboardCB(cb_key, CALLBACK_KEY_STATE_RELEASE, x, y);
    ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
}
static void _MouseCB(int button, int state, int x, int y)
{
    CALLBACK_MOUSE cb_mouse = GLUTMouseToCBMouse(button);
    CALLBACK_MOUSE_STATE cb_state = (state == GLUT_DOWN) ? CALLBACK_MOUSE_STATE_PRESS : CALLBACK_MOUSE_STATE_RELEASE;
    g_pCallbacks->SetMouseState(cb_mouse, cb_state);
    g_pCallbacks->MouseCB(cb_mouse, cb_state, x, y);
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}
static void _MouseMotionCB(int x, int y)
{
    g_pCallbacks->MouseMotionCB(x, y);
    ImGui_ImplGLUT_MotionFunc(x, y);
}

static void _MousePassiveMotionCB(int x, int y)
{
    g_pCallbacks->PassiveMouseCB(x, y);
    ImGui_ImplGLUT_MotionFunc(x, y);
}

static void _RenderSceneCB()
{
    g_pCallbacks->RenderSceneCB();
}

static void _IdleCB()
{
    g_pCallbacks->RenderSceneCB();
}

static void _ReshapeCB(int width, int height)
{
    g_pCallbacks->ReshapeCB(width, height);
    ImGui_ImplGLUT_ReshapeFunc(width, height);
}

static void _MouseWheelCB(int button, int dir, int x, int y)
{
    g_pCallbacks->MouseWheelCB(button, dir, x, y);
    ImGui_ImplGLUT_MouseWheelFunc(button, dir, x, y);
}
void InitCallbacks()
{
    glutIgnoreKeyRepeat(1);
    glutDisplayFunc(_RenderSceneCB);
    glutIdleFunc(_IdleCB);
    glutKeyboardFunc(_KeyboardCB);
    glutKeyboardUpFunc(_KeyboardUpCB);
    glutSpecialFunc(_SpecialKeyboardCB);
    glutSpecialUpFunc(_SpecialKeyboardUpCB);
    glutMouseFunc(_MouseCB);
    glutMouseWheelFunc(_MouseWheelCB);
    glutMotionFunc(_MouseMotionCB);
    glutPassiveMotionFunc(_MousePassiveMotionCB);
    glutReshapeFunc(_ReshapeCB);
}

void DeleteCallbacks()
{
    glutDisplayFunc(NULL);
    glutIdleFunc(NULL);
    glutKeyboardFunc(NULL);
    glutKeyboardUpFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpecialUpFunc(NULL);
    glutMouseFunc(NULL);
    glutMouseWheelFunc(NULL);
    glutMotionFunc(NULL);
    glutPassiveMotionFunc(NULL);
    glutReshapeFunc(NULL);
}

void RegisterICallback(ICallbacks *callback)
{
    g_pCallbacks = callback;
}

void UnregisterICallback()
{
    g_pCallbacks = &dummy_callbacks;
}