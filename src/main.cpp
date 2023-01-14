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
    }
    virtual bool Init()
    {
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        show_main_window = 1;
        m_fileDialog.SetTitle("Select a file");
        return true;
    }

    virtual bool Render()
    {
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        m_pBasicLight->Enable();

        m_pBasicLight->Disable();

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
            }
            m_fileDialog.ClearSelected();
        }

        // control panel
        {
            ImGui::Begin("main control", &show_main_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(300, WINDOW_HEIGHT), ImGuiCond_Always);
            
            ImGui::ColorEdit3("clear color", (float*)&clearColor);
            ImGui::Checkbox("camera move", &cameraMoveFlag);
            if(ImGui::Button("generate model"))
            {
                show_generate_model_window = true;
            }
            if(show_generate_model_window){
                ImGui::Separator();
                ImGui::Text("generate model");
                if (ImGui::Button("add new part")) {
                    model_part_num++;
                    model_part_pos.push_back(glm::vec3(0.0f));
                    model_part_rot.push_back(glm::vec3(0.0f));
                    model_part_mesh.push_back(Mesh());
                    return true;
                }
                for(int i = 0; i < model_part_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("part %d", i);
                    ImGui::DragFloat3("init pos", (float*)&model_part_pos[i], 0.01f, -5.0f, 5.0f);
                    ImGui::DragFloat3("init rot", (float*)&model_part_rot[i], 0.01, -5.0f, 5.0f);
                    if(ImGui::Button("Change mesh"))
                    {
                        m_fileDialogType = 0;
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.push_back(std::to_string(i));
                        m_fileDialog.SetTypeFilters({ ".obj", ".stl"});
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
    std::vector<Mesh> model_part_mesh;

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


