#ifndef _SCENE_H
#define _SCENE_H

#include "callback.h"
#include "utils.h"
#include "camera.h"
#include "pipeline.h"
#include "lighting.h"
#include "grab.h"

#include <imgui.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl2.h>
#include <windows.h>
#include <imm.h>
#pragma comment (lib ,"imm32.lib")

class Scene : public ICallbacks
{
public:
    Scene() {
        m_pCamera = nullptr;
        m_pBasicLight = nullptr;
        m_pScreenGraber = nullptr;
    }
    ~Scene() {
        SAFE_DELETE(m_pCamera);
        SAFE_DELETE(m_pBasicLight);
        SAFE_DELETE(m_pScreenGraber);
    }
    virtual bool Init() {
        return true;
    };

    virtual int Run() {
        // 将清空颜色设置为黑色
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        RegisterICallback(this);
        InitCallbacks();

        // GLUT 主循环
        glutMainLoop();
        return m_ret;
    };

    virtual bool Back() {
        return true;
    };

    virtual bool Preinit(int argc, char **argv) {
        // GLUT Init and Window Create
        glutInit(&argc, argv);
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
        glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        glutCreateWindow("OpenGL Sample");

        // Enter Game Mode(Full Screen)
        char game_mode_string[64];
        snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", 1920, 1080);
        glutGameModeString(game_mode_string);
        glutEnterGameMode();
        glutSetCursor(GLUT_CURSOR_NONE);

        // GLEW Init
        glewInit();

        // 禁用输入法
        HWND hWnd = GetActiveWindow();// 获取窗口句柄
        HIMC g_hIMC = NULL; // g_hIMC 用于恢复时使用
        g_hIMC = ImmAssociateContext(hWnd, NULL); // handle 为要禁用的窗口句柄
        // 恢复禁用
        // ImmAssociateContext(handle, hIMC); //handle 为要启用的窗口句柄 

        // ImGui Init
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGLUT_Init();
        ImGui_ImplGLUT_InstallFuncs();
        ImGui_ImplOpenGL2_Init();

        // 初始化灯光
        m_pBasicLight = new LightingTechnique();
        if (!m_pBasicLight->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }
        m_pBasicLight->Enable();
        m_pBasicLight->SetTextureUnit(0);
        m_pBasicLight->SetMatSpecularIntensity(0.0f);
        m_pBasicLight->SetMatSpecularPower(0);
        m_pBasicLight->SetPointLights(0, nullptr);
        m_pBasicLight->SetSpotLights(0, nullptr);
        m_pBasicLight->Disable();
        
        // init directionLight
        m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.4f;
        m_directionalLight.DiffuseIntensity = 0.01f;
        m_directionalLight.Direction = glm::vec3(1.0f, -1.0f, 0.0f);

        // init persParam
        m_persParam.FOV = 60.0f;
        m_persParam.Height = WINDOW_HEIGHT;
        m_persParam.Width = WINDOW_WIDTH;
        m_persParam.zNear = 1.0f;
        m_persParam.zFar = 100.0f;

        // init camera
        m_pCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
        m_pCamera->SetICallback(this);

        // init graber
        m_pScreenGraber = new ScreenGraber();
        grab_cnt = 0;
        grab_switch = 0;
        return true;
    };

    virtual bool Postinit(int argc, char **argv) {
        return true;
    };

    virtual bool PreRender() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGLUT_NewFrame();

        m_pCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pBasicLight->Enable();
        m_pBasicLight->SetDirectionalLight(m_directionalLight);
        m_pBasicLight->SetEyeWorldPos(m_pCamera->GetPos());
        m_pBasicLight->Disable();

        return true;
    };

    virtual bool Render() {
        return true;
    };

    virtual bool PostRender() {
        // 检查长按事件
        CheckKeyBoard();
        glutSwapBuffers();
        // 处理图像存储
        if(grab_switch) {
            grab_cnt++;
            if(grab_cnt > 15) {
                m_pScreenGraber->GrabScreen();
                grab_cnt = 0;
            }
        }
        return true;
    };

    virtual bool Preback(int argc, char **argv) {
        return true;
    };

    virtual bool Postback(int argc, char **argv) {
        // Imgui Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGLUT_Shutdown();
        ImGui::DestroyContext();
        return true;
    };

    virtual void RenderSceneCB() {
        PreRender();
        Render();
        PostRender();
    };

    virtual void Keyboard(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y) {

    }

    virtual void KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y)
    {
        if (KeyState == CALLBACK_KEY_STATE_PRESS) {
            switch (Key) {
                case CALLBACK_KEY_ESCAPE:
                case CALLBACK_KEY_q:
                    m_ret = 0;
                    glutLeaveMainLoop();
                    break;    
                case CALLBACK_KEY_1:
                    m_ret = 1;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_2:
                    m_ret = 2;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_3:
                    m_ret = 3;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_4:
                    m_ret = 4;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_5:
                    m_ret = 5;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_6:
                    m_ret = 6;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_7:
                    m_ret = 7;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_8:
                    m_ret = 8;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_9:
                    m_ret = 9;
                    glutLeaveMainLoop();
                    break;
                case CALLBACK_KEY_w:
                case CALLBACK_KEY_UP:
                case CALLBACK_KEY_s:
                case CALLBACK_KEY_DOWN:
                case CALLBACK_KEY_a:
                case CALLBACK_KEY_LEFT:
                case CALLBACK_KEY_d:
                case CALLBACK_KEY_RIGHT:
                case CALLBACK_KEY_z:
                case CALLBACK_KEY_PAGE_UP:
                case CALLBACK_KEY_c:
                case CALLBACK_KEY_PAGE_DOWN:
                case CALLBACK_KEY_r:
                    m_pCamera->OnKeyboard(Key);
                    break;
                case CALLBACK_KEY_p:
                    m_pScreenGraber->GrabScreen();
                    m_pScreenGraber->saveColorImg("output/test.png");
                    break;
                case CALLBACK_KEY_l:
                    grab_switch = !grab_switch;
                    break;
            }
        }
        Keyboard(Key, KeyState, x, y);
    }

    virtual void PassiveMouse(int x, int y)
    {}

    virtual void PassiveMouseCB(int x, int y)
    {
        m_pCamera->OnMouse(x, y);
        PassiveMouse(x, y);
    }

    virtual void CheckKeyBoard() {
        CALLBACK_KEY list[10] = {
        CALLBACK_KEY_w, CALLBACK_KEY_s, CALLBACK_KEY_a, CALLBACK_KEY_d, CALLBACK_KEY_z, 
        CALLBACK_KEY_c, CALLBACK_KEY_UP, CALLBACK_KEY_DOWN, CALLBACK_KEY_LEFT, CALLBACK_KEY_RIGHT
        };
        for (int i = 0; i < 10; ++i){
            if (GetKeyState(list[i]) == CALLBACK_KEY_STATE_PRESS){
                m_pCamera->OnKeyboard(list[i]);
            }
        }
        if (GetKeyState(CALLBACK_KEY_f) == CALLBACK_KEY_STATE_PRESS){
            m_directionalLight.AmbientIntensity += 0.01f;
        }
        if (GetKeyState(CALLBACK_KEY_g) == CALLBACK_KEY_STATE_PRESS){
            m_directionalLight.AmbientIntensity -= 0.01f;
        }
        if (GetKeyState(CALLBACK_KEY_v) == CALLBACK_KEY_STATE_PRESS){
            m_directionalLight.DiffuseIntensity += 0.01f;
        }
        if (GetKeyState(CALLBACK_KEY_b) == CALLBACK_KEY_STATE_PRESS){
            m_directionalLight.DiffuseIntensity -= 0.01f;
        }
    }
protected:

    Camera* m_pCamera = NULL;
    PersParam m_persParam;
    LightingTechnique* m_pBasicLight = NULL;
    DirectionalLight m_directionalLight;
    ScreenGraber *m_pScreenGraber = NULL;
    int grab_cnt;
    int grab_switch;
    int m_ret;
};

#endif