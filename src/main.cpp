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
#include "shadow_map_fbo.h"
#include "picking.h"
#include "gui/imfilebrowser.h"
#include "model/bezierface.h"

bool cameraMoveFlag = false;
class MainScene : public Scene
{
public:
    MainScene() : Scene(MODEL_CAMERA) {
    }
    ~MainScene() {
        SAFE_DELETE(m_pBase_model);
    }
    virtual bool Init()
    {
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        show_main_window = 1;
        m_fileDialog.SetTitle("Select a file");
        m_pBase_model = new SphereMesh(0.05f, 18, 36);
        m_pBase_model->buildVertices();

        m_directionalLight.DiffuseIntensity = 0.5f;

        return true;
    }

    virtual bool Render()
    {
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        // printf("CHeck\n");
        /// 渲染机械臂模型
        m_pBasicLight->Enable();
        for(int i = 0; i < model_part_num; ++i) {
            Pipeline p;
            p.Translate(model_part_pos[i].x, model_part_pos[i].y, model_part_pos[i].z);
            p.Rotate(model_part_rot[i].x, model_part_rot[i].y, model_part_rot[i].z);
            p.Scale(model_part_scale[i].x, model_part_scale[i].y, model_part_scale[i].z);
            p.SetCamera(*m_pCamera);
            p.SetPerspectiveProj(m_persParam);
            model_part_mesh[i].Render(p.GetWVPTrans(), p.GetWorldTrans(), &model_part_texture[i]);
        }
        m_pBasicLight->Disable();

        // 渲染文件管理器
        m_fileDialog.Display();
        if(m_fileDialog.HasSelected())
        {
            if(m_fileDialogType == 0) {
                // 实现选中新模型文件
                // m_fileDialogArgs[0] -> int 目标模型id
                std::stringstream ss;
                ss << m_fileDialogArgs[0];
                int id;
                ss >> id;
                if(!model_part_mesh[id].LoadMesh(m_fileDialog.GetSelected().string().c_str())){
                    printf("Load mesh %s failed!\n", m_fileDialog.GetSelected().string().c_str());
                    return false;
                }
                printf("load mesh %s success\n", m_fileDialog.GetSelected().string().c_str());
            } else if(m_fileDialogType == 1) {
                // 实现选中新模型纹理
                // m_fileDialogArgs[0] -> int 目标模型id
                std::stringstream ss;
                ss << m_fileDialogArgs[0];
                int id;
                ss >> id;
                model_part_texture[id] = Texture(GL_TEXTURE_2D, m_fileDialog.GetSelected().string().c_str());
                if (!model_part_texture[id].Load()) {
                    printf("Load mesh %s failed!\n", m_fileDialog.GetSelected().string().c_str());
                    return false;
                }
                printf("load mesh %s success\n", m_fileDialog.GetSelected().string().c_str());
            }
            m_fileDialog.ClearSelected();
        }

        // 控制面板
        {
            //窗口控制
            ImGui::Begin("main control", &show_main_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(300, WINDOW_HEIGHT), ImGuiCond_Always);
            
            // 背景色
            ImGui::ColorEdit3("clear color", (float*)&clearColor);
            ImGui::ColorEdit3("light color", (float*)&m_directionalLight.Color);
            ImGui::DragFloat("Ambient", &m_directionalLight.AmbientIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &m_directionalLight.DiffuseIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("direction", (float*)&m_directionalLight.Direction, 0.01f, -5.0f, 5.0f);

            // 相机控制
            ImGui::Checkbox("camera move", &cameraMoveFlag);
            if(ImGui::Button("Reset Camera")) {
                // reset modelCamera
                SAFE_DELETE(m_pCamera);
                if(m_camera_mode == MODEL_CAMERA) {
                    m_pCamera = new ModelCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
                }
                if(m_camera_mode == MOVE_CAMERA) {
                    m_pCamera = new MoveCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
                }
                m_pCamera->SetICallBack(this);
                m_pCamera->Init();
                return true;
            }
            if(ImGui::Button("Change Camera Type")) {
                glm::vec3 pos = m_pCamera->GetPos();
                glm::vec3 target = m_pCamera->GetTarget();
                glm::vec3 up = m_pCamera->GetUp();
                SAFE_DELETE(m_pCamera);
                if(m_camera_mode == MODEL_CAMERA) {
                    m_pCamera = new MoveCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
                    m_camera_mode = MOVE_CAMERA;
                } else {
                    m_pCamera = new ModelCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
                    m_camera_mode = MODEL_CAMERA;
                }
                m_pCamera->SetICallBack(this);
                m_pCamera->SetPos(pos);
                m_pCamera->SetTarget(target);
                m_pCamera->SetUp(up);
                SetMouse(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                return true;
            }
            ImGui::Text("Camera Mode: %s", m_camera_mode == MODEL_CAMERA ? "Model Camera" : "Move Camera");
            ImGui::DragFloat3("Camera Pos", (float*)&m_pCamera->GetPos(), 0.1f, -50.0f, 50.0f);
            ImGui::DragFloat3("Camera Target", (float*)&m_pCamera->GetTarget(), 0.1f, -50.0f, 50.0f);
            if (m_camera_mode == MODEL_CAMERA){
                if(cameraMoveFlag){
                    SetKeyState(CALLBACK_KEY_ESCAPE, CALLBACK_KEY_STATE_PRESS);
                } else {
                    SetKeyState(CALLBACK_KEY_ESCAPE, CALLBACK_KEY_STATE_RELEASE);
                }
            }
            if(ImGui::Button("generate model"))
            {
                show_generate_model_window = true;
            }
            // 建立模型面板
            if(show_generate_model_window){
                ImGui::Separator();
                ImGui::Text("generate model");
                if (ImGui::Button("add new part")) {
                    model_part_num++;
                    model_part_pos.emplace_back(glm::vec3(0.0f));
                    model_part_rot.emplace_back(glm::vec3(0.0f));
                    model_part_scale.emplace_back(glm::vec3(0.01f));
                    model_part_mesh.emplace_back(Mesh());
                    if(!model_part_mesh[model_part_num - 1].LoadMesh("mesh/base2.stl")){
                        printf("Load mesh %s failed!\n", "mesh/base2.stl");
                        return false;
                    };
                    printf("load mesh %s success\n", "mesh/base2.stl");
                    
                    model_part_texture.emplace_back(Texture(GL_TEXTURE_2D, "pic/test.png"));
                    if(!model_part_texture[model_part_num - 1].Load()){
                        printf("Load texture %s failed!\n", "pic/test.png");
                        return false;
                    }
                    printf("load texture %s success\n", "pic/test.png");
                    return true;
                }
                for(int i = 0; i < model_part_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("part %d", i);
                    ImGui::DragFloat3("init pos", (float*)&model_part_pos[i], 0.01f, -5.0f, 5.0f);
                    ImGui::DragFloat3("init rot", (float*)&model_part_rot[i], 1, -360.0f, 360.0f);
                    ImGui::DragFloat3("init scale", (float*)&model_part_scale[i], 0.01, -5.0f, 5.0f);
                    if(ImGui::Button("Change mesh"))
                    {
                        m_fileDialogType = 0;
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.emplace_back(std::to_string(i));
                        m_fileDialog.SetTypeFilters({ ".obj", ".stl"});
                        m_fileDialog.Open();
                    }
                    if(ImGui::Button("Change texture"))
                    {
                        m_fileDialogType = 1;
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.emplace_back(std::to_string(i));
                        m_fileDialog.SetTypeFilters({ ".png", ".jpg"});
                        m_fileDialog.Open();
                    }
                }
            }

            ImGui::End();            
        }
        ImGui::Render();
        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        return true;
    }
private:
    // 文件对话框类型，参数
    ImGui::FileBrowser m_fileDialog;
    int m_fileDialogType;
    std::vector<std::string> m_fileDialogArgs;

private:
    // 机器人模型组件个数，位置，姿态，模型
    int model_part_num;
    std::vector<glm::vec3> model_part_pos;
    std::vector<glm::vec3> model_part_rot;
    std::vector<glm::vec3> model_part_scale;

    std::vector<Mesh> model_part_mesh;
    std::vector<Texture> model_part_texture;


private:
    SphereMesh* m_pBase_model;

private:
    // 全局控制变量
    static bool show_main_window;
    static glm::vec4 clearColor;
private:
    static bool show_generate_model_window;
};
bool MainScene::show_main_window;
bool MainScene::show_generate_model_window;

glm::vec4 MainScene::clearColor;

int main(int argc, char **argv)
{ 
    SceneController controller;
    Scene* pMainScene = new MainScene();

    controller.AddScene(pMainScene);
    controller.Run(argc, argv);

    delete pMainScene;

    return 0;
}


