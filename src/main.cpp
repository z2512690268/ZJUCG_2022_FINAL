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
        glm::vec3 Pos(FieldWidth / 2, 1.0f, FieldDepth / 2);//初始化坐标，位于Field中间，距地面1.0f的位置
        glm::vec3 Target(0.0f, 0.0f, 1.0f);                 //初始化视角方向，指向景深
        glm::vec3 Up(0.0f, 1.0f, 0.0f);                     //初始化上方方向，垂直于Field
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
        m_pFloor->InitVertexMesh(Vertices, Indices, "pic/test.png");
        
        // init transform  param
        m_scale = 0.0f;

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

    virtual bool Render()
    {
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
        p.Rotate(90.0f, 0.0f, m_scale);
        p.Translate(-10.0f, 0.0f, 40.0f);
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);

        m_pMesh->Render(p.GetWVPTrans(), p.GetWorldTrans());

        Pipeline p2;
        p2.Translate(0.0f, 0.0f, 1.0f);
        p2.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p2.SetPerspectiveProj(m_persParam);

        m_pFloor->Render(p2.GetWVPTrans(), p2.GetWorldTrans());

        m_pSkyBox->Render();
        
        m_pBasicLight->Disable();
        return true;
    }

private:
    Mesh* m_pMesh;
    Mesh* m_pFloor;
    float m_scale;
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
        m_pFloor->InitVertexMesh(Vertices, Indices, "pic/test.png");
        
        // init transform  param
        m_scale = 0.0f;

        // init reflect param
        m_SpecularIntensiry = 0.0f;
        m_SpecularPower = 0.0f;

        return true;
    }

    virtual bool Render()
    {
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

        m_pFloor->Render(p2.GetWVPTrans(), p2.GetWorldTrans());
        m_pBasicLight->Disable();

        return true;
    }

private:
    Mesh* m_pFloor;
    float m_scale;
    float m_SpecularIntensiry;
    float m_SpecularPower;
};

class App3 : public Scene
{
public:
    App3() : Scene() {
        m_pPyramid = nullptr;
    }
    ~App3() {
        SAFE_DELETE(m_pPyramid);
    }
    virtual bool Init()
    {
        m_pPyramid = new Mesh();
        // init Plane
        const glm::vec3 Normal = glm::vec3(0.0, 1.0f, 0.0f);

        std::vector<Vertex> Vertices = { 
            Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
            Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) 
        };

        std::vector<unsigned int> Indices = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   1, 2, 0 };


        CalcVerticesNormal(Vertices, Indices);

        m_pPyramid->InitVertexMesh(Vertices, Indices, "pic/test.png");
        
        // init transform  param
        m_scale = 0.0f;

        // init reflect param
        m_SpecularIntensiry = 0.0f;
        m_SpecularPower = 0.0f;

        return true;
    }

    virtual bool Render()
    {

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

        Pipeline p;
        p.Rotate(0.0f, m_scale, 0.0f);
        p.Translate(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);

        m_pPyramid->Render(p.GetWVPTrans(), p.GetWorldTrans());
        m_pBasicLight->Disable();

        return true;
    }

private:
    Mesh* m_pPyramid;
    float m_scale;
    float m_SpecularIntensiry;
    float m_SpecularPower;
};


static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

class App4 : public Scene
{
public:
    App4() : Scene() {
        m_pPyramid = nullptr;
    }
    ~App4() {
        SAFE_DELETE(m_pPyramid);
    }

    virtual bool Init()
    {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

        m_pPyramid = new Mesh();
        // init Plane
        const glm::vec3 Normal = glm::vec3(0.0, 1.0f, 0.0f);

        std::vector<Vertex> Vertices = { 
            Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
            Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) 
        };

        std::vector<unsigned int> Indices = { 0, 3, 1,
                                   1, 3, 2,
                                   2, 3, 0,
                                   1, 2, 0 };


        CalcVerticesNormal(Vertices, Indices);

        m_pPyramid->InitVertexMesh(Vertices, Indices, "pic/test.png");
    
        // init transform  param
        m_scale = 0.0f;
        return true;
    }

    virtual bool Render()
    {
        m_pBasicLight->Enable();

        m_scale += 0.057f;

        Pipeline p;
        p.Rotate(0.0f, m_scale, 0.0f);
        p.Translate(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);

        m_pPyramid->Render(p.GetWVPTrans(), p.GetWorldTrans());
        m_pBasicLight->Disable();

        // Rendering
        my_display_code();
        ImGui::Render();

        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        return true;
    }
private:
    void my_display_code()
    {
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
    }
    Mesh* m_pPyramid;
    float m_scale;
};

class Lab5 : public Scene
{
public:
	Lab5() : Scene() {
		m_pdesk = nullptr;
		m_pleg[0] = nullptr;
		m_pleg[1] = nullptr;
		m_pleg[2] = nullptr;
		m_pleg[3] = nullptr;
	}
	~Lab5() {
		SAFE_DELETE(m_pdesk);
		SAFE_DELETE(m_pleg[0]);
		SAFE_DELETE(m_pleg[1]);
		SAFE_DELETE(m_pleg[2]);
		SAFE_DELETE(m_pleg[3]);
	}
	virtual bool Init()
	{
		m_pdesk = new Mesh();
		for (int i = 0; i < 4; i++) m_pleg[i] = new Mesh();
		// init Plane
		const glm::vec3 Normal = glm::vec3(0.0, 1.0f, 0.0f);

		// 每个mRectangle对象传递两个vec3参数，第一个参数为长方体大小，第二个为位置
		desk = new mRectangle(glm::vec3(1.0f, 1.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
		leg[0] = new mRectangle(glm::vec3(0.2f, 0.2f, 0.6f), glm::vec3(-0.2f, -0.4f, -0.2f));
		leg[1] = new mRectangle(glm::vec3(0.2f, 0.2f, 0.6f), glm::vec3(0.2f, -0.4f, -0.2f));
		leg[2] = new mRectangle(glm::vec3(0.2f, 0.2f, 0.6f), glm::vec3(-0.2f, -0.4f, 0.2f));
		leg[3] = new mRectangle(glm::vec3(0.2f, 0.2f, 0.6f), glm::vec3(0.2f, -0.4f, 0.2f));

		CalcVerticesNormal(desk->Vertices, desk->Indices);
		for (int i = 0; i < 4; i++) CalcVerticesNormal(leg[i]->Vertices, leg[i]->Indices);
		int tex_id;
		m_pdesk->InitVertexMesh(desk->Vertices, desk->Indices, "pic/Crack.bmp");
		//tex_id = m_pdesk->AddTexture("pic/Spot.bmp");
		//m_pdesk->AddMeshEntry(desk->Vertices, desk->Indices, tex_id);
		for (int i = 0; i < 4; i++)
		{
			m_pleg[i]->InitVertexMesh(leg[i]->Vertices, leg[i]->Indices, "pic/Crack.bmp");
		}

		// init transform  param
		m_scale = 0.0f;

		// init reflect param
		m_SpecularIntensiry = 0.0f;
		m_SpecularPower = 0.0f;

		return true;
	}

	virtual bool Render()
	{

		// 要引入多个效果，必须要在每个效果之前调用Enable()函数
		m_pBasicLight->Enable();

		m_scale += 0.057f;
		// init pointLight
		PointLight pl[2];
		pl[0].DiffuseIntensity = 0.25f;
		pl[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		pl[0].Position = glm::vec3(3.0f, 1.0f, FieldDepth * (cosf(m_scale) + 1.0f) / 2.0f);
		pl[0].Attenuation.Linear = 0.1f;
		pl[1].DiffuseIntensity = 0.5f;
		pl[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		pl[1].Position = glm::vec3(7.0f, 1.0f, FieldDepth * (sinf(m_scale) + 1.0f) / 2.0f);
		pl[1].Attenuation.Linear = 0.1f;
		m_pBasicLight->SetPointLights(2, pl);

		SpotLight sl[2];
		sl[0].DiffuseIntensity = 0.9f;
		sl[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
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

		Pipeline p;
		p.Rotate(0.0f, m_scale, 0.0f);
		p.Translate(0.0f, 0.0f, 3.0f);
		p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
		p.SetPerspectiveProj(m_persParam);

		m_pdesk->Render(p.GetWVPTrans(), p.GetWorldTrans());

		for(int i=0;i<4;i++) m_pleg[i]->Render(p.GetWVPTrans(), p.GetWorldTrans());
		m_pBasicLight->Disable();

		return true;
	}

private:
	Mesh* m_pdesk;
	Mesh* m_pleg[4];
	mRectangle* desk;
	mRectangle* leg[4];
	float m_scale;
	float m_SpecularIntensiry;
	float m_SpecularPower;
};

class Lab6 : public Scene
{
};

class App7 : public Scene
{
};

int main(int argc, char **argv)
{ 
    SceneController controller;
    Scene* pScene = new App1();
    Scene* pScene2 = new App2();
    Scene* pScene3 = new App3();
    Scene* pScene4 = new App4();
    Scene* pScene5 = new Lab5();
    Scene* pScene6 = new Lab6();
    Scene* pScene7 = new App7();

    controller.AddScene(pScene);
    controller.AddScene(pScene2);
    controller.AddScene(pScene3);
    controller.AddScene(pScene4);
    controller.AddScene(pScene5);
    controller.AddScene(pScene6);
    controller.AddScene(pScene7);
    controller.Run(argc, argv);

    delete pScene;
    delete pScene2;
    delete pScene3;
    delete pScene4;
    delete pScene5;
    delete pScene6;
    delete pScene7;
    return 0;
}


