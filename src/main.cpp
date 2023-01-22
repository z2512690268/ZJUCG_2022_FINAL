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
static const float FieldDepth = 50.0f;
static const float FieldWidth = 25.0f;

#define ENTRY_SCENE 1
#define MODEL_EDITOR 2
#define BAZIER_EDITOR 3
#define SCENE_EDITOR 4
#define PICKING_DEMO 5
#define SHADOW_DEMO 6

glm::vec4 clearColor;


#define MAX_MODEL_PARTS 20
#define MAX_JOINT_PARTS 50
struct JointMesh
{
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
            model_part_pos[i] = new glm::vec3(0.0f, 0.0f, 0.0f);
            model_part_rot[i] = new glm::vec3(0.0f, 0.0f, 0.0f);
            model_part_scale[i] = new glm::vec3(1.0f, 1.0f, 1.0f);
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
    glm::mat4x4 Render(const glm::mat4x4& WorldMatrix, const CameraBase& Camera, const PersParam &persParam, bool norender = false) {
        Pipeline pbase;
        glm::mat4x4 EndTransMatrix;
        pbase.SetBaseMatrix(WorldMatrix);
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
            p.SetCamera(Camera);
            p.SetPerspectiveProj(persParam);
            if(!norender){
                model_part_mesh[i]->Render(p.GetWVPTrans(), p.GetWorldTrans(), model_part_texture[i]);
            }
            EndTransMatrix = p.GetWorldTrans();
            pbase.SetBaseMatrix(p.GetWorldTrans());
            pbase.Scale(1.0f / model_part_scale[i]->x, 1.0f / model_part_scale[i]->y, 1.0f / model_part_scale[i]->z);
        }
        return EndTransMatrix;
    }
    // 机器人模型组件个数，位置，姿态，模型
    int model_part_num;
    glm::vec3* model_part_pos[MAX_MODEL_PARTS];
    glm::vec3* model_part_rot[MAX_MODEL_PARTS];
    glm::vec3* model_part_scale[MAX_MODEL_PARTS];

    Mesh*    model_part_mesh[MAX_MODEL_PARTS];
    std::string model_part_mesh_name[MAX_MODEL_PARTS];
    Texture* model_part_texture[MAX_MODEL_PARTS];
    std::string model_part_texture_name[MAX_MODEL_PARTS];

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
    int model_joint_bind_type[MAX_JOINT_PARTS];
    int model_joint_bind_part[MAX_JOINT_PARTS];
    float model_joint_ctrl[MAX_JOINT_PARTS];
};

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

            if(ImGui::Button("Model&Joint Editor", ImVec2(400, 50)))
            {
                m_ret = MODEL_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::Button("Bazier Editor", ImVec2(400, 50)))
            {
                m_ret = BAZIER_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::Button("Scene Editor", ImVec2(400, 50)))
            {
                m_ret = SCENE_EDITOR;
                glutLeaveMainLoop();
            }
            if(ImGui::CollapsingHeader("Other Exhibition")) {

                if(ImGui::Button("Picking Demo", ImVec2(400, 50)))
                {
                    m_ret = PICKING_DEMO;
                    glutLeaveMainLoop();
                }
                
                if(ImGui::Button("SHADOW Demo", ImVec2(400, 50)))
                {
                    m_ret = SHADOW_DEMO;
                    glutLeaveMainLoop();
                }

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
            model_part_pos[i] = new glm::vec3(0.0f, 0.0f, 0.0f);
            model_part_rot[i] = new glm::vec3(0.0f, 0.0f, 0.0f);
            model_part_scale[i] = new glm::vec3(1.0f, 1.0f, 1.0f);
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
            // ImGui::ShowDemoWindow();
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
                    model_part_scale[model_part_num] = new glm::vec3(1.0f);
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
                    ImGui::DragFloat3(("init scale" + std::to_string(i)).c_str(), (float*)model_part_scale[i], 0.01, -100.0f, 100.0f);
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

            if (ImGui::Button("Clear all model", ImVec2(290, 0))) {
                for(int i = 0; i < model_part_num; i++)
                {
                    SAFE_DELETE(model_part_mesh[i]);
                    SAFE_DELETE(model_part_texture[i]);
                    SAFE_DELETE(model_part_pos[i]);
                    SAFE_DELETE(model_part_rot[i]);
                    SAFE_DELETE(model_part_scale[i]);
                }
                model_part_num = 0;
                model_joint_num = 0;
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

            if (ImGui::Button("Return to Entry Scene", ImVec2(290, 0))) {
                m_ret = ENTRY_SCENE;
                glutLeaveMainLoop();
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
                SaveModel(m_pfileDialog->GetSelected().string()); 
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
    int model_joint_bind_type[MAX_JOINT_PARTS];
    int model_joint_bind_part[MAX_JOINT_PARTS];
    float model_joint_ctrl[MAX_JOINT_PARTS];
};


class BazierEditor: public Scene
{
public:
    BazierEditor() : Scene(MODEL_CAMERA) {
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
    }
    ~BazierEditor() {
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pfileDialog);
        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pSphereMesh);
    }
    virtual bool Init() {
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
    void SaveModel(std::string filename = "model.obj") {
        std::ofstream fout(filename);
        for (int i = 0; i < vertices.size(); ++i) {
            fout << "v " << vertices[i].m_pos.x << " " << vertices[i].m_pos.y << " " << vertices[i].m_pos.z << std::endl;
        }
        for (int i = 0; i < vertices.size(); ++i) {
            fout << "vt " << vertices[i].m_tex.x << " " << vertices[i].m_tex.y << std::endl;
        }
        for (int i = 0; i < vertices.size(); ++i) {
            fout << "vn " << vertices[i].m_normal.x << " " << vertices[i].m_normal.y << " " << vertices[i].m_normal.z << std::endl;
        }
        for (int i = 0; i < indices.size(); i+=3) {
            fout << "f " << indices[i] + 1 << "/" << indices[i] + 1 << "/" << indices[i] + 1 << " " << indices[i+1] + 1 << "/" << indices[i+1] + 1 << "/" << indices[i+1] + 1 << " " << indices[i+2] + 1 << "/" << indices[i+2] + 1 << "/" << indices[i+2] + 1 << std::endl;
        }
        fout.close();
    }
    virtual bool Render() {
        glDisable(GL_CULL_FACE);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
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
            } else if (broswerType == 2) {
                SaveModel(m_pfileDialog->GetSelected().string().c_str());
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser();
            }
            m_pfileDialog->ClearSelected();
        }
        {
            ImGui::Begin("BazierEditor");
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
                SetKeyState(CALLBACK_KEY_ESCAPE, CALLBACK_KEY_STATE_PRESS);
            } else {
                SetKeyState(CALLBACK_KEY_ESCAPE, CALLBACK_KEY_STATE_RELEASE);
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
                
                vertices.clear();
                indices.clear();

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
            if(ImGui::Button("Save Mesh")) {
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
                m_pfileDialog->SetTypeFilters({ ".obj" });
                m_pfileDialog->Open();
                broswerType = 2;
            }
            ImGui::Text("control points in range 0-3\nand 3-6, 6-9... would be a face\nLess than 3 points would be ignored");
            if(ImGui::CollapsingHeader("CtrlPoints")){
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
            }
            if(ImGui::Button("Add CtrlPoint")) {
                ctrlPoints.push_back(glm::vec2(0.0f, 0.0f));
                ++ctrlPointNum;
            }
            if (ImGui::Button("Return to Entry Scene", ImVec2(290, 0))) {
                m_ret = ENTRY_SCENE;
                glutLeaveMainLoop();
            }
            ImGui::End();
        }

        ImGui::Render();
        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        return true;
    }
private:
    Mesh* m_pMesh;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ImGui::FileBrowser* m_pfileDialog;
    Texture* m_pTexture;
    Mesh* m_pSphereMesh;
private:
    int broswerType;
    glm::vec4 clearColor;
    int ctrlPointNum;
    std::vector<glm::vec2> ctrlPoints;
    bool showCtrlPoint;
    bool showMesh;
};

#define MAX_SCENE_MODEL 20
class SceneEditor : public Scene
{
public:
    SceneEditor() : Scene(MODEL_CAMERA) {
        m_pSkyBox = nullptr;
    }
    ~SceneEditor() {
        SAFE_DELETE(m_pSkyBox);
    }
    virtual bool Init() {
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        m_pfileDialog = new ImGui::FileBrowser();
        m_pfileDialog->SetTitle("Select a file");

        m_directionalLight.DiffuseIntensity = 0.5f;
        m_persParam.zFar = 1000.0f;

        showSkyBox = true;

        m_pSkyBox = new SkyBox(m_pCamera, m_persParam);

        reverse_base = glm::mat4x4(1.0f);
        model_mesh_reverse.LoadModel("output/robot_rev.model");

        if (!m_pSkyBox->Init("pic",
                "starsbox_right1.png",
                "starsbox_left2.png",
                "starsbox_top3.png",
                "starsbox_bottom4.png",
                "starsbox_front5.png",
                "starsbox_back6.png"
                )) {
            printf("SkyBox Init Failed\n");
            return false;
        }  

        LoadScene("output/testscene2.scene");
        return true;
    }
    void SaveScene(std::string filename = "scene.scene") {
        std::ofstream fout(filename);
        fout << model_num << std::endl;
        for(int i = 0; i < model_num; ++i) {
            fout << model_mesh_name[i] << std::endl;
            fout << model_pos[i]->x << " " << model_pos[i]->y << " " << model_pos[i]->z << std::endl;
            fout << model_rot[i]->x << " " << model_rot[i]->y << " " << model_rot[i]->z << std::endl;
            fout << model_scale[i]->x << " " << model_scale[i]->y << " " << model_scale[i]->z << std::endl;
            for(int j = 0; j < model_mesh[i]->model_joint_num; ++j) {
                fout << model_mesh[i]->model_joint_ctrl[j] << " ";
            }
            fout << std::endl;
        }
    }
    void LoadScene(std::string filename = "scene.scene") {
        std::ifstream fin(filename);
        fin >> model_num;
        for(int i = 0; i < model_num; ++i) {
            fin >> model_mesh_name[i];
            model_pos[i] = new glm::vec3();
            model_rot[i] = new glm::vec3();
            model_scale[i] = new glm::vec3();
            fin >> model_pos[i]->x >> model_pos[i]->y >> model_pos[i]->z;
            fin >> model_rot[i]->x >> model_rot[i]->y >> model_rot[i]->z;
            fin >> model_scale[i]->x >> model_scale[i]->y >> model_scale[i]->z;
            model_mesh[i] = new JointMesh();
            model_mesh[i]->LoadModel(model_mesh_name[i]);
            for(int j = 0; j < model_mesh[i]->model_joint_num; ++j) {
                fin >> model_mesh[i]->model_joint_ctrl[j];
            }
        }

    }
    bool ImGuiPanel() {
        // 控制面板
        {
            // ImGui::ShowDemoWindow();
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
                ImGui::Checkbox("show skybox", &showSkyBox);
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
            if (ImGui::CollapsingHeader("scene editor")){
                ImGui::Separator();
                ImGui::Text("model editor");
                if (ImGui::Button("load new model") && model_num < MAX_SCENE_MODEL){
                    model_num++;
                    model_pos[model_num - 1] = new glm::vec3(0.0f, 0.0f, 0.0f);
                    model_rot[model_num - 1] = new glm::vec3(0.0f, 0.0f, 0.0f);
                    model_scale[model_num - 1] = new glm::vec3(1.0f, 1.0f, 1.0f);
                    model_mesh[model_num - 1] = new JointMesh();
                    model_mesh_name[model_num - 1] = "";
                    ImGui::End();
                    return true;
                }
                for(int i = 0; i < model_num; i++)
                {
                    ImGui::Separator();
                    ImGui::Text("part %d", i);
                    ImGui::DragFloat3(("model pos " + std::to_string(i)).c_str(), (float*)model_pos[i], 0.01f, -100.0f, 100.0f);
                    ImGui::DragFloat3(("model rot " + std::to_string(i)).c_str(), (float*)model_rot[i], 1, -720.0f, 720.0f);
                    ImGui::DragFloat3(("model scale" + std::to_string(i)).c_str(), (float*)model_scale[i], 0.01, -100.0f, 100.0f);
                    if(ImGui::Button(("change model " + std::to_string(i)).c_str())){
                        m_pfileDialog->Open();
                        m_fileDialogType = 0;
                        m_pfileDialog->SetTypeFilters({ ".model" });
                        m_fileDialogArgs.clear();
                        m_fileDialogArgs.push_back(std::to_string(i));
                        return true;
                    }
                }
            }

            if(ImGui::CollapsingHeader("model joint control")) {
                for (int i = 0; i < model_num; ++i) {
                    ImGui::Separator();
                    std::string mesh_name = "mesh " + std::to_string(i);
                    if(ImGui::CollapsingHeader(mesh_name.c_str())) {
                        for (int j = 0; j < model_mesh[i]->model_joint_num; ++j) {
                            ImGui::Text("joint %d", j);
                            ImGui::DragFloat(("joint angle " + std::to_string(j)).c_str(), &model_mesh[i]->model_joint_ctrl[j], 1, -720.0f, 720.0f);
                        }
                    }
                    if(model_mesh_name[i] == "output\\robot.model") {
                        if(ImGui::Button("robot reverse")) {
                            Pipeline base;
                            base.SetBaseMatrix(reverse_base);
                            base.Translate(model_pos[i]->x, model_pos[i]->y, model_pos[i]->z);
                            base.Rotate(model_rot[i]->x, model_rot[i]->y, model_rot[i]->z);
                            base.Scale(model_scale[i]->x, model_scale[i]->y, model_scale[i]->z);
                            if(!showReverseJoints) {
                                reverse_base = model_mesh[i]->Render(base.GetWorldTrans(), *m_pCamera, m_persParam, true);
                                for(int j = 0; j < model_mesh[i]->model_joint_num; ++j) {
                                    // model_joint_ctrl[j] = model_mesh[i]->model_joint_ctrl[model_mesh[i]->model_joint_num - 1 - j];
                                }
                            } else {
                                reverse_base = model_mesh_reverse.Render(base.GetWorldTrans(), *m_pCamera, m_persParam, true);
                                for(int j = 0; j < model_mesh[i]->model_joint_num; ++j) {
                                    model_mesh[i]->model_joint_ctrl[j] = model_mesh_reverse.model_joint_ctrl[model_mesh[i]->model_joint_num - 1 - j];
                                }
                            }
                            *model_pos[i] = glm::vec3(0.0f, 0.0f, 0.0f);
                            *model_rot[i] = glm::vec3(0.0f, 0.0f, 0.0f);
                            *model_scale[i] = glm::vec3(1.0f, 1.0f, 1.0f);
                            showReverseJoints = !showReverseJoints;
                        }
                    }
                }
                
            }
            if (ImGui::Button("Save Scene", ImVec2(290, 0))) {
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
                m_fileDialogType = 2;
                m_fileDialogArgs.clear();
                m_pfileDialog->SetTypeFilters({ ".scene"});
                m_pfileDialog->Open();
            }
            if (ImGui::Button("Load Scene", ImVec2(290, 0))) {
                m_fileDialogType = 3;
                m_fileDialogArgs.clear();
                m_pfileDialog->SetTypeFilters({ ".scene"});
                m_pfileDialog->Open();
            }
            ImGui::Text("Application average %.3f ms/frame\n(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            if (ImGui::Button("Return to Entry Scene", ImVec2(290, 0))) {
                m_ret = ENTRY_SCENE;
                glutLeaveMainLoop();
            }
            ImGui::End();            
        }
        return true;
    }

    virtual bool Render()
    {
        // printf("Check1\n");
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        // printf("CHeck\n");
        /// 渲染机械臂模型
        // printf("Check8\n");
        m_pBasicLight->Enable();
        for(int i = 0; i < model_num; i++)
        {
            Pipeline pbase;
            pbase.Translate(model_pos[i]->x, model_pos[i]->y, model_pos[i]->z);
            pbase.Rotate(model_rot[i]->x, model_rot[i]->y, model_rot[i]->z);
            pbase.Scale(model_scale[i]->x, model_scale[i]->y, model_scale[i]->z);

            model_mesh[i]->Render(pbase.GetWorldTrans(), *m_pCamera, m_persParam);
        }


        // printf("Check4\n");
        if(showSkyBox)
        {
            m_pSkyBox->Render();
        }
        m_pBasicLight->Disable();

        // 渲染文件管理器
        m_pfileDialog->Display();
        if(m_pfileDialog->HasSelected())
        {
            if(m_fileDialogType == 0) {
                int index = std::stoi(m_fileDialogArgs[0]);
                SAFE_DELETE(model_mesh[index]);
                model_mesh[index] = new JointMesh();
                SAFE_DELETE(model_pos[index]);
                model_pos[index] = new glm::vec3(0.0f, 0.0f, 0.0f);
                SAFE_DELETE(model_rot[index]);
                model_rot[index] = new glm::vec3(0.0f, 0.0f, 0.0f);
                SAFE_DELETE(model_scale[index]);
                model_scale[index] = new glm::vec3(1.0f, 1.0f, 1.0f);
                model_mesh[index]->LoadModel(m_pfileDialog->GetSelected().string().c_str());
                model_mesh_name[index] = m_pfileDialog->GetSelected().string();
            } else if (m_fileDialogType == 2) {
                // save scene
                SaveScene(m_pfileDialog->GetSelected().string().c_str());
                SAFE_DELETE(m_pfileDialog);
                m_pfileDialog = new ImGui::FileBrowser();
            } else if (m_fileDialogType == 3) {
                // load scene
                LoadScene(m_pfileDialog->GetSelected().string().c_str());
            }
            m_pfileDialog->ClearSelected();
        }

        // printf("Check3\n");
        // printf("Check9\n");
        int ret = ImGuiPanel();
        if (!ret) return false;
        ImGui::Render();
        // // Draw
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        // printf("Check2\n");
        return true;
    }
private:
    SkyBox* m_pSkyBox;
    bool showSkyBox;
    bool showReverseJoints;
private:
    ImGui::FileBrowser* m_pfileDialog;
    int m_fileDialogType;
    std::vector<std::string> m_fileDialogArgs;
private:
    int model_num;
    JointMesh* model_mesh[MAX_SCENE_MODEL]; 
    JointMesh model_mesh_reverse;
    glm::mat4x4 reverse_base;
    std::string model_mesh_name[MAX_SCENE_MODEL];
    glm::vec3* model_pos[MAX_SCENE_MODEL];
    glm::vec3* model_rot[MAX_SCENE_MODEL];
    glm::vec3* model_scale[MAX_SCENE_MODEL];
};


class PickingScene : public Scene
{
public:
    PickingScene(void) : Scene(MOVE_CAMERA), sphereCount(36), m_sectorCount(36), m_stackCount(18)
    {
        m_predtexture = nullptr;
        return;
    }
    ~PickingScene(void)
    {
        SAFE_DELETE(m_predtexture);
        return;
    }

    virtual bool Init(void)
    {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

        m_scale = 0;
        SphereMesh sphere;
        sphere.set(0.2, m_sectorCount, m_stackCount);
        sphere.buildVertices();
        PyramidMesh pyramid(0.1f, 15.0f, 0.4f);

        m_directionalLight.DiffuseIntensity = 0.5f;

        m_sphere.InitVertexMesh(sphere.getVertices(), sphere.getIndices(), "pic/orange.jpg");
        m_pyraid.InitVertexMesh(pyramid.Vertices, pyramid.Indices, "pic/007FFF.jpg");

        if (!m_pickingTexture.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }
        
        if (!m_pickingEffect.Init()) {
            return false;
        }

        m_predtexture = new Texture(GL_TEXTURE_2D, "pic/red.jpg");
        if (!m_predtexture->Load()) {
            return false;
        }

        m_selectedId = 1;
        return true;
    }

    void PickPass(void)
    {
        m_pickingTexture.EnableWriting();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pickingEffect.Enable();

        int k = 0;
        Pipeline tp;
        tp.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        tp.SetPerspectiveProj(m_persParam);

        for(int i = -3; i < 3; i++){
            for(int j = -3; j < 3; j++)
            {
                tp.Translate(i * 0.5, 0, j * 0.5);
                m_pickingEffect.SetWVP(tp.GetWVPTrans());
                ++k;

                m_pickingEffect.SetObjectIndex(k);
                m_sphere.Render(tp.GetWVPTrans(), tp.GetWorldTrans());
                
                tp.Translate(i * 0.5, 0.2f, j * 0.5);
                m_pickingEffect.SetWVP(tp.GetWVPTrans());
                ++k;

                m_pickingEffect.SetObjectIndex(k);
                m_pyraid.Render(tp.GetWVPTrans(), tp.GetWorldTrans());

            }
        }

        m_pickingEffect.Disable();
        m_pickingTexture.DisableWriting();     
    }

    void RenderPass(void)
    {
        m_pBasicLight->Enable();
        int k = 0;
        Pipeline tp;
        tp.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        tp.SetPerspectiveProj(m_persParam);

        for(int i = -3; i < 3; i++){
            for(int j = -3; j < 3; j++)
            {
                tp.Translate(i * 0.5, 0, j * 0.5);
                ++k;
                if (k != m_selectedId){
                    m_sphere.Render(tp.GetWVPTrans(), tp.GetWorldTrans());
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    m_sphere.Render(tp.GetWVPTrans(), tp.GetWorldTrans(), m_predtexture);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                tp.Translate(i * 0.5, 0.2f, j * 0.5);
                ++k;
                if (k != m_selectedId){
                    m_pyraid.Render(tp.GetWVPTrans(), tp.GetWorldTrans());
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    m_pyraid.Render(tp.GetWVPTrans(), tp.GetWorldTrans(), m_predtexture);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }

        m_pBasicLight->Disable();
    }

    virtual bool Render(void)
    {
        PickPass();
        RenderPass();
        return true;
    }

    virtual void MouseCB(CALLBACK_MOUSE Button, CALLBACK_MOUSE_STATE State, int x, int y) {
        if (Button == CALLBACK_MOUSE_BUTTON_LEFT && State == CALLBACK_MOUSE_STATE_PRESS) {
            PixelInfo Pixel = m_pickingTexture.ReadPixel(x, GAMEMODE_WINDOW_HEIGHT - y);
            m_selectedId = Pixel.ObjectID;
        }
    };

    
private:
    Mesh m_sphere;
    Mesh m_pyraid;
    float m_radius;
    int m_sectorCount;
    int m_stackCount;
    const int sphereCount;
    PickingTexture m_pickingTexture;
    PickingTechnique m_pickingEffect;
    Texture* m_predtexture;
    int m_selectedId;
    float m_scale;
};

class ShadowDemo : public Scene
{
public:
    ShadowDemo() : Scene(MOVE_CAMERA) {
        m_pMesh = nullptr;
        m_pFloor = nullptr;
        m_pSkyBox = nullptr;
        m_pShadowMapEffect = nullptr;
    }
    ~ShadowDemo() {
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pFloor);
        SAFE_DELETE(m_pSkyBox);
        SAFE_DELETE(m_pShadowMapEffect);
    }
    virtual bool Init()
    {
        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        m_pShadowMapEffect = new ShadowMapTechnique();
        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }
        // init spotlight mesh
        SphereMesh sphere;
        sphere.set(1.0, 10, 10);
        sphere.buildVertices();
        m_pSphereMesh = new Mesh();
        m_pSphereMesh->InitVertexMesh(sphere.getVertices(), sphere.getIndices(), "pic/sun.jpg");


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

    void ShadowMapPass()
    {
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        m_pBasicLight->Enable();
        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(90.0f, 0.0f, m_scale);
        p.Translate(FieldWidth / 2, 10.0f, FieldDepth / 2);
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, glm::vec3(0.0f, 1.0f, 0.0f));
        p.SetPerspectiveProj(m_persParam);
        m_pBasicLight->SetLightWVP(p.GetWVPTrans());
        m_pBasicLight->Disable();

        m_pShadowMapEffect->Enable();
        m_pShadowMapEffect->SetWVP(p.GetWVPTrans());
        m_pMesh->Render(p.GetWVPTrans(), p.GetWorldTrans());

        m_pShadowMapEffect->Disable();
        m_shadowMapFBO.UnbindForWriting();
    }

    void RenderPass()
    {
        m_pBasicLight->Enable();

        m_shadowMapFBO.BindForReading(GL_TEXTURE1);

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(90.0f, 0.0f, m_scale);
        p.Translate(FieldWidth / 2, 10.0f, FieldDepth / 2);
        p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p.SetPerspectiveProj(m_persParam);
        m_pMesh->Render(p.GetWVPTrans(), p.GetWorldTrans());
        
        Pipeline p2;
        p2.Translate(0.0f, 0.0f, 1.0f);
        p2.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p2.SetPerspectiveProj(m_persParam);
        m_pFloor->Render(p2.GetWVPTrans(), p2.GetWorldTrans());

        Pipeline p3;
        p3.Translate(m_spotLight.Position.x, m_spotLight.Position.y, m_spotLight.Position.z);
        p3.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
        p3.SetPerspectiveProj(m_persParam);
        m_pSphereMesh->Render(p3.GetWVPTrans(), p3.GetWorldTrans());

        m_pSkyBox->Render();

        m_shadowMapFBO.UnbindForReading(GL_TEXTURE1);
        m_pBasicLight->Disable();
    }

    virtual bool Render()
    {
        m_scale += 0.057f;

        ShadowMapPass();
        RenderPass();

        return true;
    }

private:
    Mesh* m_pMesh;
    Mesh* m_pFloor;
    float m_scale;
    SkyBox* m_pSkyBox;
    Mesh* m_pSphereMesh;

    SpotLight m_spotLight;
    ShadowMapFBO m_shadowMapFBO;

    ShadowMapTechnique* m_pShadowMapEffect;
};


int main(int argc, char **argv)
{ 
    SceneController controller;
    Scene* pEntryScene = new EntryScene();
    Scene* pModelEditorScene = new ModelEditor();
    Scene* pExtraScene = new BazierEditor();
    Scene* pSceneEditorScene = new SceneEditor();
    Scene* pPickingScene = new PickingScene();
    Scene* pShadowMapScene = new ShadowDemo();

    controller.AddScene(pEntryScene);
    controller.AddScene(pModelEditorScene);
    controller.AddScene(pExtraScene);
    controller.AddScene(pSceneEditorScene);
    controller.AddScene(pPickingScene);
    controller.AddScene(pShadowMapScene);

    controller.Run(argc, argv);

    delete pEntryScene;
    delete pModelEditorScene;
    delete pExtraScene;
    delete pSceneEditorScene;

    return 0;
}


