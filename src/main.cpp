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
#define MAX_MODEL_PARTS 20
#define MAX_JOINT_PARTS 50
bool cameraMoveFlag = false;
static const float FieldDepth = 50.0f;
static const float FieldWidth = 25.0f;

#define ENTRY_SCENE 1
#define MODEL_EDITOR 2
#define JOINT_EDITOR 3
#define SCENE_EDITOR 4
#define SCENE_ROAM 5

glm::vec4 clearColor;

class EntryScene : public Scene
{
public:
    EntryScene() : Scene(MODEL_CAMERA) {
    } 
    ~EntryScene() {
    }
    virtual bool Init() {
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        return true;
    }
    virtual bool Render() {
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        {
            ImGui::Begin("Entry Window", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowPos(ImVec2(500, 250), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(415, 400), ImGuiCond_Always);
            if(ImGui::CollapsingHeader("Help")) {
                ImGui::Text("This is a simple 3D model editor.");
                ImGui::Text("You can edit the model, joint and scene.");
                ImGui::Text("You can also roam the scene.");
                ImGui::Text("Any time, you can press 'q' to quit.");
                ImGui::Text("Enjoy it!");
            }

            if(ImGui::Button("Model Editor", ImVec2(400, 50)))
            {
                m_ret = MODEL_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::Button("Joint Editor", ImVec2(400, 50)))
            {
                m_ret = JOINT_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::Button("Scene Editor", ImVec2(400, 50)))
            {
                m_ret = SCENE_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::Button("Scene Roaming", ImVec2(400, 50)))
            {
                m_ret = SCENE_ROAM;
                glutLeaveMainLoop();
            }

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        return true;
    }
};

// 编辑器场景， 用于编辑与导出模型
class ModelEditor : public Scene
{
public:
    ModelEditor() : Scene(MODEL_CAMERA) {
    }
    ~ModelEditor() {
        // SAFE_DELETE(m_pSkyBox);
    }
    virtual bool Init()
    {
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        m_pfileDialog = new ImGui::FileBrowser();
        m_pfileDialog->SetTitle("Select a file");

        m_directionalLight.DiffuseIntensity = 0.5f;

        model_part_num = 8;
        model_part_mesh[0] = new Mesh();
        model_part_mesh[0]->LoadMesh("mesh/base2.stl");
        model_part_mesh_name[0] = "mesh/base2.stl";
        model_part_pos[0] = new glm::vec3(0.0f, 0.0f, 0.0f);
        model_part_rot[0] = new glm::vec3(0.0f, 0.0f, 0.0f);
        model_part_scale[0] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[0] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[0] = "pic/test.png";
        model_part_texture[0]->Load();

        model_part_mesh[1] = new Mesh();
        model_part_mesh[1]->LoadMesh("mesh/short2.stl");
        model_part_mesh_name[1] = "mesh/short2.stl";
        model_part_pos[1] = new glm::vec3(0.0f, 0.75f, 0.0f);
        model_part_rot[1] = new glm::vec3(0.0f, 0.0f, 0.0f);
        model_part_scale[1] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[1] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[1] = "pic/test.png";
        model_part_texture[1]->Load();

        model_part_mesh[2] = new Mesh();
        model_part_mesh[2]->LoadMesh("mesh/short2.stl");
        model_part_mesh_name[2] = "mesh/short2.stl";
        model_part_pos[2] = new glm::vec3(0.0f, 0.5f, 0.5f);
        model_part_rot[2] = new glm::vec3(-90.0f, -90.0f, 0.0f);
        model_part_scale[2] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[2] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[2] = "pic/test.png";
        model_part_texture[2]->Load();

        model_part_mesh[3] = new Mesh();
        model_part_mesh[3]->LoadMesh("mesh/long.stl");
        model_part_mesh_name[3] = "mesh/long.stl";
        model_part_pos[3] = new glm::vec3(0.0f, 0.5f, 0.5f);
        model_part_rot[3] = new glm::vec3(0.0f, -90.0f, -90.0f);
        model_part_scale[3] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[3] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[3] = "pic/test.png";
        model_part_texture[3]->Load();

        model_part_mesh[4] = new Mesh();
        model_part_mesh[4]->LoadMesh("mesh/long.stl");
        model_part_mesh_name[4] = "mesh/long.stl";
        model_part_pos[4] = new glm::vec3(0.0f, 1.0f, 5.0f);
        model_part_rot[4] = new glm::vec3(0.0f, 180.0f, 0.0f);
        model_part_scale[4] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[4] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[4] = "pic/test.png";
        model_part_texture[4]->Load();

        model_part_mesh[5] = new Mesh();
        model_part_mesh[5]->LoadMesh("mesh/short2.stl");
        model_part_mesh_name[5] = "mesh/short2.stl";
        model_part_pos[5] = new glm::vec3(0.0f, 1.0f, 5.0f);
        model_part_rot[5] = new glm::vec3(0.0f, -90.0f, 0.0f);
        model_part_scale[5] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[5] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[5] = "pic/test.png";
        model_part_texture[5]->Load();

        model_part_mesh[6] = new Mesh();
        model_part_mesh[6]->LoadMesh("mesh/short2.stl");
        model_part_mesh_name[6] = "mesh/short2.stl";
        model_part_pos[6] = new glm::vec3(0.0f, 0.5f, 0.5f);
        model_part_rot[6] = new glm::vec3(-90.0f, 90.0f, 0.0f);
        model_part_scale[6] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[6] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[6] = "pic/test.png";
        model_part_texture[6]->Load();

        model_part_mesh[7] = new Mesh();
        model_part_mesh[7]->LoadMesh("mesh/base2.stl");
        model_part_mesh_name[7] = "mesh/base2.stl";
        model_part_pos[7] = new glm::vec3(0.0f, 0.5f, 1.25f);
        model_part_rot[7] = new glm::vec3(90.0f, 0.0f, 0.0f);
        model_part_scale[7] = new glm::vec3(0.01f, 0.01f, 0.01f);
        model_part_texture[7] = new Texture(GL_TEXTURE_2D, "pic/test.png");
        model_part_texture_name[7] = "pic/test.png";
        model_part_texture[7]->Load();

        model_joint_num = 7;
        for(int i = 0; i < model_joint_num; ++i) {
            model_joint_bind_type[i] = JOINT_TYPE_ROTATE_Y;
            model_joint_bind_part[i] = i + 1;
        }
        return true;
    }

    void SaveModel(std::string filename = "output/test.model") {
        // save model
        std::ofstream fout(filename);
        fout << model_part_num << std::endl;
        for(int i = 0; i < model_part_num; ++i) {
            fout << model_part_mesh_name[i] << std::endl;
            fout << model_part_pos[i]->x << " " << model_part_pos[i]->y << " " << model_part_pos[i]->z << std::endl;
            fout << model_part_rot[i]->x << " " << model_part_rot[i]->y << " " << model_part_rot[i]->z << std::endl;
            fout << model_part_scale[i]->x << " " << model_part_scale[i]->y << " " << model_part_scale[i]->z << std::endl;
            fout << model_part_texture_name[i] << std::endl;
        }
        fout << model_joint_num << std::endl;
        for(int i = 0; i < model_joint_num; ++i) {
            fout << model_joint_bind_type[i] << " " << model_joint_bind_part[i] << std::endl;
        }
        fout.close();
    }

    void LoadModel(std::string filename = "output/test.model") {
        // load model
        std::ifstream fin(filename);
        fin >> model_part_num;
        for(int i = 0; i < model_part_num; ++i) {
            fin >> model_part_mesh_name[i];
            fin >> model_part_pos[i]->x >> model_part_pos[i]->y >> model_part_pos[i]->z;
            fin >> model_part_rot[i]->x >> model_part_rot[i]->y >> model_part_rot[i]->z;
            fin >> model_part_scale[i]->x >> model_part_scale[i]->y >> model_part_scale[i]->z;
            fin >> model_part_texture_name[i];
            SAFE_DELETE(model_part_mesh[i]);
            model_part_mesh[i] = new Mesh();
            model_part_mesh[i]->LoadMesh(model_part_mesh_name[i]);
            SAFE_DELETE(model_part_texture[i]);
            model_part_texture[i] = new Texture(GL_TEXTURE_2D, model_part_texture_name[i]);
            model_part_texture[i]->Load();
        }
        fin >> model_joint_num;
        for(int i = 0; i < model_joint_num; ++i) {
            fin >> model_joint_bind_type[i] >> model_joint_bind_part[i];
        }
        fin.close();
    }

    bool ImGuiPanel() {
        // 控制面板
        {
            ImGui::ShowDemoWindow();
            //窗口控制
            ImGui::Begin("main control", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(300, WINDOW_HEIGHT), ImGuiCond_Always);
            
            // // 背景色
            if (ImGui::CollapsingHeader("light control")){
                ImGui::ColorEdit3("clear color", (float*)&clearColor);
                ImGui::ColorEdit3("light color", (float*)&m_directionalLight.Color);
                ImGui::DragFloat("Ambient", &m_directionalLight.AmbientIntensity, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Diffuse", &m_directionalLight.DiffuseIntensity, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("direction", (float*)&m_directionalLight.Direction, 0.01f, -10.0f, 10.0f);
            }
            // 相机控制
            if (ImGui::CollapsingHeader("camera control")){
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
                ImGui::SameLine();
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
            }
            // 建立模型面板
            if (ImGui::CollapsingHeader("model editor")){
                ImGui::Separator();
                ImGui::Text("model editor");
                if (ImGui::Button("add new part") && model_part_num < MAX_MODEL_PARTS) {
                    model_part_pos[model_part_num] = new glm::vec3(0.0f);
                    model_part_rot[model_part_num] = new glm::vec3(0.0f);
                    model_part_scale[model_part_num] = new glm::vec3(0.01f);
                    model_part_mesh[model_part_num] = new Mesh();
                    model_part_mesh[model_part_num]->LoadMesh("mesh/cube.obj");
                    model_part_mesh_name[model_part_num] = "mesh/cube.obj";
                    model_part_texture[model_part_num] = new Texture(GL_TEXTURE_2D, "pic/test.png");
                    model_part_texture_name[model_part_num] = "pic/test.png";
                    model_part_texture[model_part_num]->Load();
                    model_part_num++;
                    printf("Check1 %d\n", model_part_num);
                    ImGui::End();  
                    return true;
                }
                for(int i = 0; i < model_part_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("part %d", i);
                    ImGui::DragFloat3(("init pos " + std::to_string(i)).c_str(), (float*)model_part_pos[i], 0.01f, -10.0f, 10.0f);
                    ImGui::DragFloat3(("init rot " + std::to_string(i)).c_str(), (float*)model_part_rot[i], 1, -360.0f, 360.0f);
                    ImGui::DragFloat3(("init scale" + std::to_string(i)).c_str(), (float*)model_part_scale[i], 0.01, -10.0f, 10.0f);
                    if(ImGui::Button(("Change mesh" + std::to_string(i)).c_str()))
                    {
                        m_fileDialogType = 0;
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.emplace_back(std::to_string(i));
                        m_pfileDialog->SetTypeFilters({ ".obj", ".stl"});
                        m_pfileDialog->Open();
                    }
                    if(ImGui::Button(("Change texture"+ std::to_string(i)).c_str()))
                    {
                        m_fileDialogType = 1;
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.emplace_back(std::to_string(i));
                        m_pfileDialog->SetTypeFilters({ ".png", ".jpg"});
                        m_pfileDialog->Open();
                    }
                }
            }

            if (ImGui::CollapsingHeader("joint editor")) {
                ImGui::Separator();
                ImGui::Text("joint editor");
                if (ImGui::Button("add new joint") && model_joint_num < MAX_JOINT_PARTS) {
                    model_joint_num++;
                    model_joint_bind_type[model_joint_num - 1] = 0;
                    model_joint_bind_part[model_joint_num - 1] = 0;
                    return true;
                }
                for(int i = 0; i < model_joint_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("joint %d", i);
                    // combo box
                    const char* items[] = { "part0", "part1", "part2", "part3", "part4", "part5", "part6", "part7", "part8", "part9" };
                    ImGui::Combo(("bind part" + std::to_string(i)).c_str(), &model_joint_bind_part[i], items, model_part_num);
                    // type
                    ImGui::RadioButton(("rotate x " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_ROTATE_X); 
                    ImGui::SameLine();
                    ImGui::RadioButton(("translate x " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_TRANSLATE_X); 
                    ImGui::RadioButton(("rotate y " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_ROTATE_Y); 
                    ImGui::SameLine();
                    ImGui::RadioButton(("translate y " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_TRANSLATE_Y); 
                    ImGui::RadioButton(("rotate z " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_ROTATE_Z); 
                    ImGui::SameLine();
                    ImGui::RadioButton(("translate z " + std::to_string(i)).c_str(), &model_joint_bind_type[i], JOINT_TYPE_TRANSLATE_Z);
                }
            }

            if (ImGui::CollapsingHeader("joint control")) {
                ImGui::Separator();
                ImGui::Text("joint control editor");
                for(int i = 0; i < model_joint_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("joint %d", i);
                    if(model_joint_bind_type[i] == JOINT_TYPE_ROTATE_X || model_joint_bind_type[i] == JOINT_TYPE_ROTATE_Y || model_joint_bind_type[i] == JOINT_TYPE_ROTATE_Z) {
                        ImGui::DragFloat(("angle" + std::to_string(i)).c_str(), &model_joint_ctrl[i], 1, -720.0f, 720.0f);
                    }
                    else if(model_joint_bind_type[i] == JOINT_TYPE_TRANSLATE_X || model_joint_bind_type[i] == JOINT_TYPE_TRANSLATE_Y || model_joint_bind_type[i] == JOINT_TYPE_TRANSLATE_Z) {
                        ImGui::DragFloat(("translate" + std::to_string(i)).c_str(), &model_joint_ctrl[i], 0.01f, -10.0f, 10.0f);
                    }
                }
            }

            if (ImGui::Button("Save", ImVec2(290, 0))) {
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
                m_fileDialogType = 2;
                m_fileDialogArgs.clear();
                m_pfileDialog->SetTypeFilters({ ".model"});
                m_pfileDialog->Open();
            }

            if (ImGui::Button("Load", ImVec2(290, 0))) {
                m_fileDialogType = 3;
                m_fileDialogArgs.clear();
                m_pfileDialog->SetTypeFilters({ ".model"});
                m_pfileDialog->Open();
            }
            ImGui::Text("Application average %.3f ms/frame\n(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();            
        }
        return true;
    }

    virtual bool Render()
    {
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        // printf("CHeck\n");
        /// 渲染机械臂模型
        // printf("Check8\n");
        m_pBasicLight->Enable();
        Pipeline pbase;
        for(int i = 0; i < model_part_num; ++i) {
            Pipeline p;
            p.SetBaseMatrix(pbase.GetWorldTrans());
            glm::vec3 trans = glm::vec3(0.0f);
            glm::vec3 rots = glm::vec3(0.0f);
            for(int j = 0; j < model_joint_num; ++j) {
                if(model_joint_bind_part[j] == i){
                    if(model_joint_bind_type[j] == JOINT_TYPE_ROTATE_X) rots.x += model_joint_ctrl[j];
                    else if(model_joint_bind_type[j] == JOINT_TYPE_ROTATE_Y) rots.y += model_joint_ctrl[j];
                    else if(model_joint_bind_type[j] == JOINT_TYPE_ROTATE_Z) rots.z += model_joint_ctrl[j];
                    else if(model_joint_bind_type[j] == JOINT_TYPE_TRANSLATE_X) trans.x += model_joint_ctrl[j];
                    else if(model_joint_bind_type[j] == JOINT_TYPE_TRANSLATE_Y) trans.y += model_joint_ctrl[j];
                    else if(model_joint_bind_type[j] == JOINT_TYPE_TRANSLATE_Z) trans.z += model_joint_ctrl[j]; 
                }
            }
            p.Translate(model_part_pos[i]->x, model_part_pos[i]->y, model_part_pos[i]->z);
            p.Rotate(model_part_rot[i]->x, model_part_rot[i]->y, model_part_rot[i]->z);
            p.SetBaseMatrix(p.GetWorldTrans());
            p.Translate(trans.x, trans.y, trans.z);
            p.Rotate(rots.x, rots.y, rots.z);
            p.Scale(model_part_scale[i]->x, model_part_scale[i]->y, model_part_scale[i]->z);
            p.SetCamera(*m_pCamera);
            p.SetPerspectiveProj(m_persParam);
            model_part_mesh[i]->Render(p.GetWVPTrans(), p.GetWorldTrans(), model_part_texture[i]);
            pbase.SetBaseMatrix(p.GetWorldTrans());
            pbase.Scale(1.0f / model_part_scale[i]->x, 1.0f / model_part_scale[i]->y, 1.0f / model_part_scale[i]->z);
        }

        m_pBasicLight->Disable();

        // 渲染文件管理器
        m_pfileDialog->Display();
        if(m_pfileDialog->HasSelected())
        {
            if(m_fileDialogType == 0) {
                // 实现选中新模型文件
                // m_fileDialogArgs[0] -> int 目标模型id
                std::stringstream ss;
                ss << m_fileDialogArgs[0];
                int id;
                ss >> id;
                SAFE_DELETE(model_part_mesh[id]);
                model_part_mesh[id] = new Mesh();
                model_part_mesh_name[id] = m_pfileDialog->GetSelected().string();
                if(!model_part_mesh[id]->LoadMesh(m_pfileDialog->GetSelected().string().c_str())){
                    printf("Load mesh %s failed!\n", m_pfileDialog->GetSelected().string().c_str());
                    return false;
                }
                printf("load mesh %s success\n", m_pfileDialog->GetSelected().string().c_str());
            } else if(m_fileDialogType == 1) {
                // 实现选中新模型纹理
                // m_fileDialogArgs[0] -> int 目标模型id
                std::stringstream ss;
                ss << m_fileDialogArgs[0];
                int id;
                ss >> id;
                SAFE_DELETE(model_part_texture[id]);
                model_part_texture[id] = new Texture(GL_TEXTURE_2D, m_pfileDialog->GetSelected().string().c_str());
                model_part_texture_name[id] = m_pfileDialog->GetSelected().string();
                if (!model_part_texture[id]->Load()) {
                    printf("Load mesh %s failed!\n", m_pfileDialog->GetSelected().string().c_str());
                    return false;
                }
                printf("load mesh %s success\n", m_pfileDialog->GetSelected().string().c_str());
            } else if(m_fileDialogType == 2) {
                // 选中需要保存的文件名

                SaveModel(m_pfileDialog->GetSelected().string() + ".model"); 
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser();
            } else if(m_fileDialogType == 3) {
                // 选中需要读取的文件名
                LoadModel(m_pfileDialog->GetSelected().string());
            }
            m_pfileDialog->ClearSelected();
        }

        // printf("Check9\n");
        int ret = ImGuiPanel();
        if (!ret) return false;
        ImGui::Render();
        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        return true;
    }
private:
    // 文件对话框类型，参数
    ImGui::FileBrowser* m_pfileDialog;
    int m_fileDialogType;
    std::vector<std::string> m_fileDialogArgs;

private:
    // 机器人模型组件个数，位置，姿态，模型
    int model_part_num;
    glm::vec3* model_part_pos[MAX_MODEL_PARTS];
    glm::vec3* model_part_rot[MAX_MODEL_PARTS];
    glm::vec3* model_part_scale[MAX_MODEL_PARTS];

    Mesh*    model_part_mesh[MAX_MODEL_PARTS];
    std::string model_part_mesh_name[MAX_MODEL_PARTS];
    Texture* model_part_texture[MAX_MODEL_PARTS];
    std::string model_part_texture_name[MAX_MODEL_PARTS];

private:
    // 机器人关节个数，绑定的模型变量
    enum JOINT_TYPE {
        JOINT_TYPE_ROTATE_X = 0,
        JOINT_TYPE_ROTATE_Y,
        JOINT_TYPE_ROTATE_Z,
        JOINT_TYPE_TRANSLATE_X,
        JOINT_TYPE_TRANSLATE_Y,
        JOINT_TYPE_TRANSLATE_Z
    };
    int model_joint_num;
    int model_joint_bind_type[MAX_MODEL_PARTS];
    int model_joint_bind_part[MAX_MODEL_PARTS];
    float model_joint_ctrl[MAX_MODEL_PARTS];

private:
};


class ExtraScene: public Scene
{
public:
    ExtraScene() : Scene(MODEL_CAMERA) {
        broswerType = 0;
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        showCtrlPoint = false;
        showMesh = true;
        ctrlPoints = std::vector<glm::vec2>({
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.6f, 0.8f),
            glm::vec2(0.8f, 0.6f),
            glm::vec2(0.6f, 0.4f),
            glm::vec2(0.4f, 0.2f),
            glm::vec2(0.2f, 0.0f),
            glm::vec2(0.0f, 0.0f),
        });
        ctrlPointNum = ctrlPoints.size();

        m_pMesh = nullptr;
        m_pfileDialog = nullptr;
        m_pTexture = nullptr;
        m_pSphereMesh = nullptr;
        m_pShadowMapEffect = nullptr;
    }
    ~ExtraScene() {
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pfileDialog);
        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pSphereMesh);
        SAFE_DELETE(m_pShadowMapEffect);
    }
    virtual bool Init() {
        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        // init spot light
        m_spotLight.AmbientIntensity = 0.1f;
        m_spotLight.DiffuseIntensity = 0.9f;
        m_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
        m_spotLight.Attenuation.Linear = 0.1f;
        m_spotLight.Position = glm::vec3(FieldWidth / 2 + 30.0f, 40.0f, FieldDepth / 2);
        m_spotLight.Direction = glm::vec3(-1.0f, -1.0f, 0.0f);
        m_spotLight.Cutoff = 20.0f;

        m_pBasicLight->Enable();
        m_pBasicLight->SetSpotLights(1, &m_spotLight);
        m_pBasicLight->Disable();

        m_pShadowMapEffect = new ShadowMapTechnique();
        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }
        RectangleMesh rectangle(glm::vec3(1.0f, 1.0f, 1.0f));
        m_pMesh = new Mesh();
        m_pMesh->InitVertexMesh(rectangle.Vertices, rectangle.Indices, "pic/test.png");

        m_pfileDialog = new ImGui::FileBrowser();
        m_pfileDialog->SetTitle("title");

        m_pTexture = new Texture(GL_TEXTURE_2D, "pic/test.png");
        if (!m_pTexture->Load()) {
            return false;
        }

        SphereMesh sphere;
        sphere.set(0.05, 36, 18);
        sphere.buildVertices();

        m_directionalLight.DiffuseIntensity = 0.5f;

        m_pSphereMesh = new Mesh();
        m_pSphereMesh->InitVertexMesh(sphere.getVertices(), sphere.getIndices(), "pic/orange.jpg");

        return true;
    }
    void ShadowMapPass() {
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);        
    }
    void RenderPass() {
        m_pBasicLight->Enable();
        Pipeline p;
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);
        if(showMesh){
            m_pMesh->Render(p.GetWVPTrans(), p.GetWorldTrans(), m_pTexture);
        }
        if(showCtrlPoint){
            for(int i = 0; i < ctrlPointNum; ++i) {
                Pipeline p2;
                p2.Translate(ctrlPoints[i].x, ctrlPoints[i].y, 0.0);
                p2.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
                p2.SetPerspectiveProj(m_persParam);
                m_pSphereMesh->Render(p2.GetWVPTrans(), p2.GetWorldTrans());
            }
        }
        m_pBasicLight->Disable();
    }
    virtual bool Render() {
        glDisable(GL_CULL_FACE);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        ShadowMapPass();
        RenderPass();

        m_pfileDialog->Display();
        if (m_pfileDialog->HasSelected()) {
            if(broswerType == 0) {
                SAFE_DELETE(m_pTexture);
                m_pTexture = new Texture(GL_TEXTURE_2D, m_pfileDialog->GetSelected().string().c_str());
                if (!m_pTexture->Load()) {
                    return false;
                }
            } else if (broswerType == 1) {
                SAFE_DELETE(m_pMesh);
                m_pMesh = new Mesh();
                if (!m_pMesh->LoadMesh(m_pfileDialog->GetSelected().string().c_str())) {
                    return false;
                }
                printf("load mesh %s success\n", m_pfileDialog->GetSelected().string().c_str());
            }
            m_pfileDialog->ClearSelected();
        }
        {
            ImGui::Begin("ExtraScene");
            // Display some text (you can use a format string too)
            ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color
            // Edit 1 float using a slider from 0.0f to 1.0f    
            if(ImGui::Button("Reset Camera")) {
                // reset modelCamera
                SAFE_DELETE(m_pCamera);
                m_pCamera = new ModelCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
                m_pCamera->SetICallBack(this);
                m_pCamera->Init();
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("camera move", &cameraMoveFlag);
            ImGui::Checkbox("show ctrl points", &showCtrlPoint);
            ImGui::Checkbox("show mesh", &showMesh);
            if(cameraMoveFlag){
                SetKeyState(CALLBACK_KEY_w, CALLBACK_KEY_STATE_PRESS);
            } else {
                SetKeyState(CALLBACK_KEY_w, CALLBACK_KEY_STATE_RELEASE);
            }
            if(ImGui::Button("Change Texture")) {
                m_pfileDialog->SetTypeFilters({ ".png", ".jpg", ".bmp" });
                m_pfileDialog->Open();
                broswerType = 0;
            }
            ImGui::Text("Please Click the following button at first to update mesh");
            if(ImGui::Button("Update Mesh By CtrlPoints")) {
                SAFE_DELETE(m_pMesh);
                m_pMesh = new Mesh();
                
                std::vector<Vertex> vertices;
                std::vector<unsigned int> indices;

                int cur_index = 0;
                for (int i = 0; i + 3 < ctrlPoints.size(); i += 3) {
                    std::vector<glm::vec2> temp_ctrlPoints(ctrlPoints.begin() + i, ctrlPoints.begin() + i + 4);

                    BezierFace bezierFace(temp_ctrlPoints);
                    for(int i = 0; i < bezierFace.getVertices().size(); ++i) {
                        vertices.push_back(Vertex(bezierFace.getVertices()[i], bezierFace.getTexCoords()[i], bezierFace.getNormals()[i]));
                    }
                    for(int i = 0; i < bezierFace.getIndices().size(); ++i) {
                        indices.push_back(bezierFace.getIndices()[i] + cur_index);
                    }
                    cur_index = vertices.size();
                }
                m_pMesh->InitVertexMesh(vertices, indices, "pic/test.png");
            }
            ImGui::Text("control points in range 0-3\nand 3-6, 6-9... would be a face\nLess than 3 points would be ignored");
            for(int i = 0; i < ctrlPointNum; ++i) {
                ImGui::DragFloat2(("ctrlPoint" + std::to_string(i)).c_str(), (float*)&ctrlPoints[i], 0.01f, -5.0f, 5.0f);
                if(ImGui::Button(("delete ctrlPoint" + std::to_string(i)).c_str())) {
                    ctrlPoints.erase(ctrlPoints.begin() + i);
                    --ctrlPointNum;
                    break;
                }
                if(i > 0 && i % 3 == 0) {
                    ImGui::Separator();
                    ImGui::Text("Face part %d", i / 3);
                }
            }
            if(ImGui::Button("Add CtrlPoint")) {
                ctrlPoints.push_back(glm::vec2(0.0f, 0.0f));
                ++ctrlPointNum;
            }
            ImGui::End();
        }

        ImGui::Render();
        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        return true;
    }
    Mesh* m_pMesh;
    ImGui::FileBrowser* m_pfileDialog;
    Texture* m_pTexture;
    Mesh* m_pSphereMesh;
    int broswerType;
    glm::vec4 clearColor;
    int ctrlPointNum;
    std::vector<glm::vec2> ctrlPoints;
    bool showCtrlPoint;
    SpotLight m_spotLight;
    ShadowMapFBO m_shadowMapFBO;
    ShadowMapTechnique* m_pShadowMapEffect;
    bool showMesh;
};


int main(int argc, char **argv)
{ 
    SceneController controller;
    Scene* pEntryScene = new EntryScene();
    Scene* pModelEditorScene = new ModelEditor();
    Scene* pExtraScene = new ExtraScene();

    controller.AddScene(pEntryScene);
    controller.AddScene(pModelEditorScene);
    controller.AddScene(pExtraScene);

    controller.Run(argc, argv);

    delete pEntryScene;
    delete pModelEditorScene;
    delete pExtraScene;

    return 0;
}


