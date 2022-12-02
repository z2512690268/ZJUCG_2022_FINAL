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
#include "app.h"
#include "lighting.h"
#include "debug.h"

// 宏定义
#define WINDOW_WIDTH 1536
#define WINDOW_HEIGHT 864

struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;

    Vertex() {}

    Vertex(glm::vec3 pos, glm::vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

class MainApp: public App
{
public:
    MainApp()
    {
        m_pGameCamera = NULL;
        m_pTexture = NULL;
        m_pEffect = NULL;
        m_scale = 0.0f;
        m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.5f;

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;
    }

    ~MainApp()
    {
        delete m_pEffect;
        delete m_pGameCamera;
        delete m_pTexture;
    }

    bool Init()
    {
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

        CreateVertexBuffer();
        CreateIndexBuffer();
        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            return false;
        }

        m_pEffect->Enable();

        m_pEffect->SetTextureUnit(0);

        m_pTexture = new Texture(GL_TEXTURE_2D, "pic/test.png");

        if (!m_pTexture->Load()) {
            return false;

        }
        return true;
    }

    virtual void RenderSceneCB()
    {
        std::cout << "RenderSceneCB" << std::endl;
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT);

        m_scale += 0.1f;

        Pipeline p;
        p.Rotate(0.0f, m_scale, 0.0f);
        p.Translate(0.0f, 0.0f, 3.0f);
        p.SetCamera(*m_pGameCamera);
        p.SetPerspectiveProj(m_persProjInfo);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetDirectionalLight(m_directionalLight);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        m_pTexture->Bind(GL_TEXTURE0);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        // gluSphere(gluNewQuadric(), 1.0f, 30, 30);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glutSwapBuffers();
    }

    virtual void KeyboardCB(CALLBACK_KEY Key, CALLBACK_KEY_STATE KeyState, int x, int y)
    {
        std::cout << "KeyboardCB" << KeyState << std::endl;
        if (KeyState == CALLBACK_KEY_STATE_PRESS) {
            switch (Key) {
                case CALLBACK_KEY_ESCAPE:
                case CALLBACK_KEY_q:
                    exit(0);
                    break;

                case CALLBACK_KEY_f:
                    // m_directionalLight.AmbientIntensity += 0.05f;
                    break;

                case CALLBACK_KEY_g:
                    // m_directionalLight.AmbientIntensity -= 0.05f;
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
                    m_pGameCamera->OnKeyboard(Key);
                    break;
            }
        }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
        std::cout << "PassiveMouseCB" << std::endl;
        // m_pGameCamera->OnMouse(x, y);
    }
private:

    void CreateVertexBuffer()
    {
        Vertex Vertices[4] = { 
            Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
            Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) 
        };

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void CreateIndexBuffer()
    {
        unsigned int Indices[] = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   1, 2, 0 };

        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    }

    GLuint m_VBO;
    GLuint m_IBO;
    LightingTechnique* m_pEffect;
    Texture* m_pTexture;
    Camera* m_pGameCamera;
    float m_scale;
    DirectionalLight m_directionalLight;
    PersParam m_persProjInfo;
};

int main(int argc, char **argv)
{
    // GLUT Init and Window Create
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample");

    // char game_mode_string[64];
    // snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", 1920, 1080);
    // glutGameModeString(game_mode_string);
    // glutEnterGameMode();
    // glutSetCursor(GLUT_CURSOR_NONE);

    // GLEW Init
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }
  
    MainApp* pApp = new MainApp();

    if (!pApp->Init())
    {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}


