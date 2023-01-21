#include "scene.h"

Scene::Scene(CAMERA_MODE camera_mode) {
    m_pCamera = nullptr;
    m_pBasicLight = nullptr;
    m_pScreenGraber = nullptr;
    m_camera_mode = camera_mode;
}
Scene::~Scene() {
    SAFE_DELETE(m_pCamera);
    SAFE_DELETE(m_pBasicLight);
    SAFE_DELETE(m_pScreenGraber);
}

int Scene::Run() {
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

bool Scene::Preinit(int argc, char **argv) {
    // GLUT Init and Window Create
    glutInit(&argc, argv);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL Sample");

    // Enter Game Mode(Full Screen)
    char game_mode_string[64];
    snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", GAMEMODE_WINDOW_WIDTH, GAMEMODE_WINDOW_HEIGHT);
    glutGameModeString(game_mode_string);
    glutEnterGameMode();

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
    m_pBasicLight->SetShadowMapTextureUnit(1);
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
    if (m_camera_mode == MOVE_CAMERA){
        m_pCamera = new MoveCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
        m_pCamera->SetICallBack(this);
        m_pCamera->Init();
    } else if (m_camera_mode == MODEL_CAMERA) {
        // init modelCamera
        m_pCamera = new ModelCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
        m_pCamera->SetICallBack(this);
        m_pCamera->Init();
    }

    // init graber
    m_pScreenGraber = new ScreenGraber();
    screen_grab_cnt = 0;
    screen_grab_switch = 0;
    camera_cnt = 0;
    m_pRealityGrabber = new RealityGrabber();
    return true;
};

bool Scene::PreRender() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    m_pCamera->OnRender();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pBasicLight->Enable();
    m_pBasicLight->SetDirectionalLight(m_directionalLight);
    m_pBasicLight->SetEyeWorldPos(m_pCamera->GetPos());
    m_pBasicLight->Disable();

    // printf("Check7\n");
    return true;
};

bool Scene::PostRender() {
    // 检查长按事件
    // printf("Check2\n");
    CheckKeyBoard();
    // printf("Check3\n");
    glutSwapBuffers();
    // printf("Check4\n");
    // 处理图像存储
    if(screen_grab_switch) {
        screen_grab_cnt++;
        if(screen_grab_cnt > 15) {
            m_pScreenGraber->GrabScreen();
            screen_grab_cnt = 0;
        }
    }
    // printf("Check5\n");
    if(camera_switch) {
        camera_cnt++;
        if(camera_cnt > 3){
            m_pRealityGrabber->GrabReality();
            camera_cnt = 0;
        }
    }
    // printf("Check6\n");
    return true;
};

bool Scene::Postback(int argc, char **argv) {
    // Imgui Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
    return true;
};

void Scene::RenderSceneCB() {
    PreRender();
    Render();
    PostRender();
};

void Scene::KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y)
{
    if (KeyState == CALLBACK_KEY_STATE_PRESS) {
        switch (Key) {
            case CALLBACK_KEY_ESCAPE:
            case CALLBACK_KEY_q:
                m_ret = 0;
                glutLeaveMainLoop();
                break;    
            // case CALLBACK_KEY_1:
            //     m_ret = 1;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_2:
            //     m_ret = 2;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_3:
            //     m_ret = 3;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_4:
            //     m_ret = 4;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_5:
            //     m_ret = 5;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_6:
            //     m_ret = 6;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_7:
            //     m_ret = 7;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_8:
            //     m_ret = 8;
            //     glutLeaveMainLoop();
            //     break;
            // case CALLBACK_KEY_9:
            //     m_ret = 9;
            //     glutLeaveMainLoop();
            //     break;
            case CALLBACK_KEY_p:
                m_pScreenGraber->GrabScreen();
                m_pScreenGraber->saveColorImg("output/test.png");
                break;
            case CALLBACK_KEY_l:
                screen_grab_switch = !screen_grab_switch;
                break;
            case CALLBACK_KEY_o:
                if(camera_switch){
                    m_pRealityGrabber->saveRealityImg("output/real.png");
                }
                break;
            case CALLBACK_KEY_k:
                if(!camera_switch){
                    m_pRealityGrabber->OpenReality();
                    camera_switch = 1;
                    camera_cnt = 0;
                }else {
                    m_pRealityGrabber->CloseReality();
                    camera_switch = 0;
                }
                break;
            case CALLBACK_KEY_m:
                if(!camera_switch){
                    m_pRealityGrabber->OpenReality();
                    m_pRealityGrabber->GrabReality();
                }
                int ret = m_handDetector.Handpose_Recognition(m_pRealityGrabber->getRealityImg());
                if(ret > 0){
                    m_ret = ret;
                    m_pRealityGrabber->CloseReality();
                    glutLeaveMainLoop();
                }
                break;
        }
    }
    m_pCamera->OnKeyboard(Key);
    Keyboard(Key, KeyState, x, y);
}

void Scene::MouseWheelCB(int button, int dir, int x, int y)
{
    m_pCamera->OnWheel(dir);
}

void Scene::PassiveMouseCB(int x, int y)
{
    m_pCamera->OnMouse(x, y);
    PassiveMouse(x, y);
}

void Scene::MouseMotionCB(int x, int y)
{
    m_pCamera->OnMouse(x, y);
}

void Scene::CheckKeyBoard() {
    m_pCamera->CheckKeyboard();
    
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