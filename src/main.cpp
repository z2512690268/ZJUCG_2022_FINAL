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


GLuint VBO;
GLuint IBO;
FileManager fileManager;
GLuint gWVPLocation;
GLuint gSampler;
Texture* pTexture = NULL;

Camera* pCamera = NULL;
PersParam persParam;

static void SpecialKeyboardCB(int Key, int x, int y)
{
    pCamera->OnSpecialKey(Key);
}


static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;
        case 'w':
        case 's':
        case 'a':
        case 'd':
        case 'z':
        case 'c':
        case 'r':
        case ' ':
            pCamera->OnKeyboard(Key);
            break;
        default:
            break;
    }
}

static void PassiveMouseCB(int x, int y)
{
    pCamera->OnMouse(x, y);
}

void RenderSceneCB()
{
    pCamera->OnRender();

    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

    Scale += 0.1f;

    Pipeline p;

    p.Rotate(0.0f, Scale, 0.0f);
    p.Translate(0.0f, 0.0f, 3.0f);
    p.SetCamera(*pCamera);
    p.SetPerspectiveProj(persParam);

    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)&p.GetWVPTrans());

    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    pTexture->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    // gluSphere(gluNewQuadric(), 1.0f, 30, 30);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutSwapBuffers();
}


static void CreateVertexBuffer()
{
    Vertex Vertices[4] = { 
        Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) 
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);  
}

static void CreateIndexBuffer()
{
    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

const char* pVSFileName = "shader/shader.vs";
const char* pFSFileName = "shader/shader.fs";

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::string vs, fs;

    if (!fileManager.ReadFile(pVSFileName, vs)) {
        exit(1);
    };

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!fileManager.ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);
    gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    assert(gSampler != 0xFFFFFFFF);

}

int main(int argc, char **argv)
{
    // GLUT Init and Window Create
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample");

    char game_mode_string[64];
    snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", 1920, 1080);
    glutGameModeString(game_mode_string);
    glutEnterGameMode();
    glutSetCursor(GLUT_CURSOR_NONE);

    pCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

    // GLEW Init
    glewInit();

    // 将清空颜色设置为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);


    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    glUniform1i(gSampler, 0);

    pTexture = new Texture(GL_TEXTURE_2D, "pic/test.png");

    if (!pTexture->Load()) {
        return 1;
    }

    persParam.FOV = 60.0f;
    persParam.Height = WINDOW_HEIGHT;
    persParam.Width = WINDOW_WIDTH;
    persParam.zNear = 1.0f;
    persParam.zFar = 100.0f;

    // 渲染函数
    glutDisplayFunc(RenderSceneCB);

    // Idle函数
    glutIdleFunc(RenderSceneCB);

    //
    glutSpecialFunc(SpecialKeyboardCB);
    glutPassiveMotionFunc(PassiveMouseCB);
    glutKeyboardFunc(KeyboardCB);

    // GLUT 主循环
    glutMainLoop();
    return 0;
}


