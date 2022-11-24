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
#include <stb_image.h>


// 自定义头文件 
#include "file.h"

// 宏定义
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

static GLuint VBO;
FileManager fileManager;
GLuint gWorldLocation;

void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

    Scale += 0.001f;

    glm::mat4x4 World(sinf(Scale),          0.0f, 0.0f, 0.0f,
                      0.0f,          sinf(Scale), 0.0f, 0.0f,
                      0.0f,                 0.0f, 1.0f, 0.0f,
                      0.0f,                 0.0f, 0.0f, 1.0f);


    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World[0][0]);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}


static void CreateVertexBuffer()
{
    glm::vec3 Vertices[3];
    Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);   // bottom left
    Vertices[1] = glm::vec3(1.0f, -1.0f, 0.0f);    // bottom right
    Vertices[2] = glm::vec3(0.0f, 1.0f, 0.0f);     // top

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);  
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

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);

}

int main(int argc, char **argv)
{
    // GLUT Init and Window Create
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Sample");

    // GLEW Init
    glewInit();

    // 将清空颜色设置为黑色
    glClearColor(0.0f,0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    // 渲染函数
    glutDisplayFunc(RenderSceneCB);

    // Idle函数
    glutIdleFunc(RenderSceneCB);

    // GLUT 主循环
    glutMainLoop();
    return 0;
}


