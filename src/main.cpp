// 标准头文件
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
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
#include "mesh.h"
#include "skybox.h"
#include "utils.h"
#include "scene.h"
#include "scenectrl.h"

static const float FieldDepth = 50.0f;
static const float FieldWidth = 25.0f;

class App1 : public Scene
{
public:
    App1() : Scene() {
        m_pMesh = nullptr;
        m_pFloor = nullptr;
        m_pSkyBox = nullptr;
    }
    ~App1() {
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pFloor);
        SAFE_DELETE(m_pSkyBox);
    }
    virtual bool Init()
    {
        // init camera
        glm::vec3 Pos(FieldWidth / 2, 1.0f, FieldDepth / 2);
        glm::vec3 Target(0.0f, 0.0f, 1.0f);
        glm::vec3 Up(0.0f, 1.0f, 0.0f); 
        m_pCamera->SetPos(Pos);
        m_pCamera->SetTarget(Target);
        m_pCamera->SetUp(Up);

       // init mesh
        m_pMesh = new Mesh();
        if (!m_pMesh->LoadMesh("mesh/phoenix_ugv.md2")) {
            return false;
        }
        m_pFloor = new Mesh();
        // init Plane
        const glm::vec3 Normal = glm::vec3(0.0, 1.0f, 0.0f);

        std::vector<Vertex> Vertices = {
                                Vertex(glm::vec3(0.0f, 0.0f, 0.0f),             glm::vec2(0.0f, 0.0f), Normal),
                                Vertex(glm::vec3(0.0f, 0.0f, FieldDepth),       glm::vec2(0.0f, 1.0f), Normal),
                                Vertex(glm::vec3(FieldWidth, 0.0f, 0.0f),       glm::vec2(1.0f, 0.0f), Normal),

                                Vertex(glm::vec3(FieldWidth, 0.0f, 0.0f),       glm::vec2(1.0f, 0.0f), Normal),
                                Vertex(glm::vec3(0.0f, 0.0f, FieldDepth),       glm::vec2(0.0f, 1.0f), Normal),
                                Vertex(glm::vec3(FieldWidth, 0.0f, FieldDepth), glm::vec2(1.0f, 1.0f), Normal)
                             };
        std::vector<unsigned int> Indices = { 0, 1, 2, 3, 4, 5 };
        int tex_id = m_pFloor->AddTexture("pic/test.png");
        m_pFloor->AddMeshEntry(Vertices, Indices, tex_id);
        
        // init transform  param
        m_scale = 0.0f;

        // init reflect param
        m_SpecularIntensiry = 0.0f;
        m_SpecularPower = 0.0f;

        // init skybox
        m_pSkyBox = new SkyBox(m_pCamera, m_persParam);

        if (!m_pSkyBox->Init("pic",
                "sp3right.jpg",
                "sp3left.jpg",
                "sp3top.jpg",
                "sp3bot.jpg",
                "sp3front.jpg",
                "sp3back.jpg")) {
            return false;
        }
        return true;
    }

    virtual void RenderSceneCB()
    {
        m_pCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 要引入多个效果，必须要在每个效果之前调用Enable()函数
        m_pBasicLight->Enable();

        m_scale += 0.057f;
        // init pointLight
        PointLight pl[2];
        pl[0].DiffuseIntensity = 0.25f;
        pl[0].Color = glm::vec3(1.0f, 0.5f, 0.0f);
        pl[0].Position = glm::vec3(3.0f, 1.0f, FieldDepth * (cosf(m_scale) + 1.0f) / 2.0f);
        pl[0].Attenuation.Linear = 0.1f;
        pl[1].DiffuseIntensity = 0.5f;
        pl[1].Color = glm::vec3(0.0f, 0.5f, 1.0f);
        pl[1].Position = glm::vec3(7.0f, 1.0f, FieldDepth * (sinf(m_scale) + 1.0f) / 2.0f);
        pl[1].Attenuation.Linear = 0.1f;
        m_pBasicLight->SetPointLights(2, pl);

        SpotLight sl[2];
        sl[0].DiffuseIntensity = 0.9f;
        sl[0].Color = glm::vec3(0.0f, 1.0f, 1.0f);
        sl[0].Position = m_pCamera->GetPos();
        sl[0].Direction = m_pCamera->GetTarget();
        sl[0].Attenuation.Linear = 0.1f;
        sl[0].Cutoff = 90.0f;

        sl[1].DiffuseIntensity = 0.9f;
        sl[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
        sl[1].Position = glm::vec3(5.0f, 3.0f, 10.0f);
        sl[1].Direction = glm::vec3(0.0f, -1.0f, 0.0f);
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 20.0f;
        m_pBasicLight->SetSpotLights(2, sl);

        // 透视投影pipeline
        Pipeline p;

        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.Translate(-10.0f, 0.0f, 40.0f);
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);

        // 传递Shader参数
        m_pBasicLight->SetWVP(p.GetWVPTrans());
        m_pBasicLight->SetWorldMatrix(p.GetWorldTrans());
        m_pBasicLight->SetDirectionalLight(m_directionalLight);
        m_pBasicLight->SetEyeWorldPos(m_pCamera->GetPos());
        m_pBasicLight->SetMatSpecularIntensity(m_SpecularIntensiry);
        m_pBasicLight->SetMatSpecularPower(m_SpecularPower);

        m_pMesh->Render();

        Pipeline p2;
        p2.Translate(0.0f, 0.0f, 1.0f);
        p2.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p2.SetPerspectiveProj(m_persParam);

        m_pBasicLight->SetWVP(p2.GetWVPTrans());
        m_pBasicLight->SetWorldMatrix(p2.GetWorldTrans());
        m_pBasicLight->SetDirectionalLight(m_directionalLight);
        m_pBasicLight->SetEyeWorldPos(m_pCamera->GetPos());
        m_pBasicLight->SetMatSpecularIntensity(0.0f);
        m_pBasicLight->SetMatSpecularPower(0);

        m_pFloor->Render();

        m_pSkyBox->Render();
        
        // 检查长按事件
        CheckKeyBoard();

        glutSwapBuffers();
    }

private:
    Mesh* m_pMesh;
    Mesh* m_pFloor;
    float m_scale;
    float m_SpecularIntensiry;
    float m_SpecularPower;
    SkyBox* m_pSkyBox;
};

class App2 : public Scene
{
public:
    App2() : Scene() {
        m_pFloor = nullptr;
    }
    ~App2() {
        SAFE_DELETE(m_pFloor);
    }
    virtual bool Init()
    {
        // init camera
        glm::vec3 Pos(FieldWidth / 2, 1.0f, FieldDepth / 2);
        glm::vec3 Target(0.0f, 0.0f, 1.0f);
        glm::vec3 Up(0.0f, 1.0f, 0.0f); 
        m_pCamera->SetPos(Pos);
        m_pCamera->SetTarget(Target);
        m_pCamera->SetUp(Up);

        m_pFloor = new Mesh();
        // init Plane
        const glm::vec3 Normal = glm::vec3(0.0, 1.0f, 0.0f);

        std::vector<Vertex> Vertices = {
                                Vertex(glm::vec3(0.0f, 0.0f, 0.0f),             glm::vec2(0.0f, 0.0f), Normal),
                                Vertex(glm::vec3(0.0f, 0.0f, FieldDepth),       glm::vec2(0.0f, 1.0f), Normal),
                                Vertex(glm::vec3(FieldWidth, 0.0f, 0.0f),       glm::vec2(1.0f, 0.0f), Normal),

                                Vertex(glm::vec3(FieldWidth, 0.0f, 0.0f),       glm::vec2(1.0f, 0.0f), Normal),
                                Vertex(glm::vec3(0.0f, 0.0f, FieldDepth),       glm::vec2(0.0f, 1.0f), Normal),
                                Vertex(glm::vec3(FieldWidth, 0.0f, FieldDepth), glm::vec2(1.0f, 1.0f), Normal)
                             };
        std::vector<unsigned int> Indices = { 0, 1, 2, 3, 4, 5 };
        int tex_id = m_pFloor->AddTexture("pic/test.png");
        m_pFloor->AddMeshEntry(Vertices, Indices, tex_id);
        
        // init transform  param
        m_scale = 0.0f;

        // init reflect param
        m_SpecularIntensiry = 0.0f;
        m_SpecularPower = 0.0f;

        return true;
    }

    virtual void RenderSceneCB()
    {
        m_pCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 要引入多个效果，必须要在每个效果之前调用Enable()函数
        m_pBasicLight->Enable();

        m_scale += 0.057f;
        // init pointLight
        PointLight pl[2];
        pl[0].DiffuseIntensity = 0.25f;
        pl[0].Color = glm::vec3(1.0f, 0.5f, 0.0f);
        pl[0].Position = glm::vec3(3.0f, 1.0f, FieldDepth * (cosf(m_scale) + 1.0f) / 2.0f);
        pl[0].Attenuation.Linear = 0.1f;
        pl[1].DiffuseIntensity = 0.5f;
        pl[1].Color = glm::vec3(0.0f, 0.5f, 1.0f);
        pl[1].Position = glm::vec3(7.0f, 1.0f, FieldDepth * (sinf(m_scale) + 1.0f) / 2.0f);
        pl[1].Attenuation.Linear = 0.1f;
        m_pBasicLight->SetPointLights(2, pl);

        SpotLight sl[2];
        sl[0].DiffuseIntensity = 0.9f;
        sl[0].Color = glm::vec3(0.0f, 1.0f, 1.0f);
        sl[0].Position = m_pCamera->GetPos();
        sl[0].Direction = m_pCamera->GetTarget();
        sl[0].Attenuation.Linear = 0.1f;
        sl[0].Cutoff = 90.0f;

        sl[1].DiffuseIntensity = 0.9f;
        sl[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
        sl[1].Position = glm::vec3(5.0f, 3.0f, 10.0f);
        sl[1].Direction = glm::vec3(0.0f, -1.0f, 0.0f);
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 20.0f;
        m_pBasicLight->SetSpotLights(2, sl);

        Pipeline p2;
        p2.Translate(0.0f, 0.0f, 1.0f);
        p2.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p2.SetPerspectiveProj(m_persParam);

        m_pBasicLight->SetWVP(p2.GetWVPTrans());
        m_pBasicLight->SetWorldMatrix(p2.GetWorldTrans());
        m_pBasicLight->SetDirectionalLight(m_directionalLight);
        m_pBasicLight->SetEyeWorldPos(m_pCamera->GetPos());
        m_pBasicLight->SetMatSpecularIntensity(0.0f);
        m_pBasicLight->SetMatSpecularPower(0);

        m_pFloor->Render();
        
        // 检查长按事件
        CheckKeyBoard();

        glutSwapBuffers();
    }

private:
    Mesh* m_pFloor;
    float m_scale;
    float m_SpecularIntensiry;
    float m_SpecularPower;
};


int main(int argc, char **argv)
{ 
    SceneController controller;
    Scene* pScene = new App1();
    Scene* pScene2 = new App2();

    controller.AddScene(pScene);
    controller.AddScene(pScene2);
    controller.Run(argc, argv);

    return 0;
}


