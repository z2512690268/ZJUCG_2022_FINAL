#ifndef _SCENE_H
#define _SCENE_H

#include "callback.h"
#include "utils.h"
#include "camera.h"
#include "pipeline.h"
#include "lighting.h"
#include "grab.h"
#include "aimodel/hand.h"

#include <imgui.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl2.h>
#include <windows.h>
#include <imm.h>
#pragma comment (lib ,"imm32.lib")

class Scene : public ICallbacks
{
public:
    Scene();
    ~Scene();

    //*************************************
    // 渲染全流程函数

    virtual int Run() ;

    virtual bool Preinit(int argc, char **argv);
    
    virtual bool Init() { return true; };

    virtual bool Postinit(int argc, char **argv) {
        return true;
    };

    virtual bool PreRender() ;

    virtual bool Render() {
        return true;
    };

    virtual bool PostRender() ;
    
    virtual bool Preback(int argc, char **argv) {
        return true;
    };
    
    virtual bool Back() {
        return true;
    };
    
    virtual bool Postback(int argc, char **argv);

    //*************************************
    // 继承的回调函数
    virtual void RenderSceneCB();

    virtual void KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y);

    virtual void MouseWheelCB(int button, int dir, int x, int y);
    
    virtual void PassiveMouseCB(int x, int y);
    
    virtual void MouseMotionCB(int x, int y);

    //*************************************
    // 在集成的回调函数中调用的函数
    virtual void Keyboard(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y) {}

    virtual void PassiveMouse(int x, int y) {}

    //*************************************
    // 每帧检查输入状态的函数
    virtual void CheckKeyBoard();
protected:

    CameraBase* m_pCamera = NULL;
    PersParam m_persParam;
    LightingTechnique* m_pBasicLight = NULL;
    DirectionalLight m_directionalLight;
    ScreenGraber *m_pScreenGraber = NULL;
    RealityGrabber *m_pRealityGrabber = NULL;
    int screen_grab_cnt;
    int screen_grab_switch;
    int m_ret;
    HandDetector  m_handDetector;
};

#endif