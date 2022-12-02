// 标准头文件
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>

// 环境头文件
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// 自定义头文件 
#include "file.h"
#include "camera.h"
#include "pipeline.h"
#include "texture.h"
#include "technique.h"
#include "lighting.h"

// 宏定义
#define WINDOW_WIDTH 1536
#define WINDOW_HEIGHT 864

struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(glm::vec3 pos, glm::vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};


class MainApp : public ICallbacks
{
public:
    MainApp (){
        m_pEffect = nullptr;
        m_pCamera = nullptr;
        m_pTexture = nullptr;
    }
    ~MainApp (){
        delete m_pEffect;
        delete m_pCamera;
        delete m_pTexture;
    }
    bool Init()
    {
        // init VBO & VIO
        CreateVertexBuffer();

        // init texture
        m_pTexture = new Texture(GL_TEXTURE_2D, "pic/test.png");

        if (!m_pTexture->Load()) {
            return 1;
        }

        // init effect
        m_pEffect = new LightingTechnique();
        if (!m_pEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return 1;
        }
        m_pEffect->Enable();
        m_pEffect->SetTextureUnit(0);

        // init directionLight
        m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.0f;
        m_directionalLight.DiffuseIntensity = 0.2f;
        m_directionalLight.Direction = glm::vec3(0.0f, 0.0, 1.0);

        // init persParam
        m_persParam.FOV = 60.0f;
        m_persParam.Height = WINDOW_HEIGHT;
        m_persParam.Width = WINDOW_WIDTH;
        m_persParam.zNear = 1.0f;
        m_persParam.zFar = 100.0f;

        // init camera
        glm::vec3 Pos(0.0f, 0.0f, -3.0f);
        glm::vec3 Target(0.0f, 0.0f, 1.0f);
        glm::vec3 Up(0.0f, 1.0f, 0.0f);
        m_pCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);
        m_pCamera->SetICallback(this);
        return true;
    }

    void Run()
    {
        // 将清空颜色设置为黑色
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        RegisterICallback(this);
        InitCallbacks();

        // GLUT 主循环
        glutMainLoop();
    }

    virtual void RenderSceneCB()
    {
        m_pCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT);

        static float Scale = 0.0f;

        Scale += 0.1f;

        // 透视投影pipeline
        Pipeline p;

        p.Rotate(0.0f, Scale, 0.0f);
        p.Translate(0.0f, 0.0f, 1.0f);
        p.SetCamera(*m_pCamera);
        p.SetPerspectiveProj(m_persParam);

        // 传递Shader参数
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetEyeWorldPos(m_pCamera->GetPos());
        m_pEffect->SetMatSpecularIntensity(1.0f);
        m_pEffect->SetMatSpecularPower(32);

        // 传递顶点信息
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
        // 绑定顶点索引
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

        // 绑定纹理
        m_pTexture->Bind(GL_TEXTURE0);
        
        // 绘制
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // 结束传递顶点信息
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        
        // 检查长按事件
        CheckKeyBoard();

        glutSwapBuffers();
    }

    virtual void KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y)
    {
        if (KeyState == CALLBACK_KEY_STATE_PRESS) {
            switch (Key) {
                case CALLBACK_KEY_ESCAPE:
                case CALLBACK_KEY_q:
                    exit(0);
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
            }
        }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
        m_pCamera->OnMouse(x, y);
    }
private:
    void CheckKeyBoard() {
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
    void CreateVertexBuffer()
    {
        Vertex Vertices[4] = { 
            Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
            Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) 
        };

        unsigned int Indices[] = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   1, 2, 0 };

        int VertexCnt = sizeof(Vertices) / sizeof(Vertex);

        for(int i = 0; i < VertexCnt; i += 3) {
            unsigned int Index1 = Indices[i];
            unsigned int Index2 = Indices[i + 1];
            unsigned int Index3 = Indices[i + 2];
            glm::vec3 norm = CalculateNormal(Vertices[Index1].m_pos, Vertices[Index2].m_pos, Vertices[Index3].m_pos);
            Vertices[Index1].m_normal += norm;
            Vertices[Index2].m_normal += norm;
            Vertices[Index3].m_normal += norm;
        }

        for(int i = 0; i < VertexCnt; i++) {
            glm::normalize(Vertices[i].m_normal);
        }

        CreateIndexBuffer(Indices, sizeof(Indices));

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void CreateIndexBuffer(unsigned int Indices[], unsigned int Size)
    {
        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, Indices, GL_STATIC_DRAW);
    }

    GLuint m_VBO;
    GLuint m_IBO;
    Texture* m_pTexture;
    LightingTechnique* m_pEffect = NULL;
    PersParam m_persParam;
    DirectionalLight m_directionalLight;
    Camera* m_pCamera = NULL;
};

int main(int argc, char **argv)
{
    // GLUT Init and Window Create
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample");

    // Enter Game Mode(Full Screen)
    char game_mode_string[64];
    snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", 1920, 1080);
    glutGameModeString(game_mode_string);
    glutEnterGameMode();
    glutSetCursor(GLUT_CURSOR_NONE);

    // GLEW Init
    glewInit();

    // App Init & Run
    MainApp* pApp = new MainApp();
    if (!pApp->Init())
    {
        return 1;
    }
    pApp->Run();
    return 0;
}


