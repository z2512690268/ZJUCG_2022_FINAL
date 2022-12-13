#include <iostream>
#include "camera.h"
#include "mathfunc.h"
#include "callback.h"
#include "utils.h"

CameraBase::CameraBase(int WindowWidth, int WindowHeight)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos          = glm::vec3(0.0f, 0.0f, 0.0f);
    m_target       = glm::vec3(0.0f, 0.0f, 1.0f);
    m_up           = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pCallback    = nullptr;
    
}

CameraBase::CameraBase(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    glm::normalize(m_target);

    m_up = Up;
    glm::normalize(m_up);
}

void MoveCamera::Init()
{
    glm::vec3 HTarget(m_target.x, 0.0, m_target.z);
    glm::normalize(HTarget);

    if (HTarget.z >= 0.0f)
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = 360.0f - glm::degrees(asin(HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f + glm::degrees(asin(HTarget.z));
        }
    }
    else
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = glm::degrees(asin(-HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f - glm::degrees(asin(-HTarget.z));
        }
    }

    m_AngleV = -glm::degrees(asin(m_target.y));

    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge  = false;
    m_OnRightEdge = false;
    m_mousePos.x  = m_windowWidth / 2;
    m_mousePos.y  = m_windowHeight / 2;
}

bool MoveCamera::OnKeyboard(CALLBACK_KEY Key) {
    bool Ret = false;
    glm::vec3 Direction = m_target;
    switch (Key) {
    case CALLBACK_KEY_UP:
    case CALLBACK_KEY_w:
        Direction.y = 0.0f;
        glm::normalize(Direction);
        m_pos += (Direction * STEP_SCALE);
        Ret = true;
        break;
    case CALLBACK_KEY_DOWN:
    case CALLBACK_KEY_s:
        Direction.y = 0.0f;
        glm::normalize(Direction);
        m_pos -= (Direction * STEP_SCALE);
        Ret = true;
        break;
    case CALLBACK_KEY_LEFT:
    case CALLBACK_KEY_a:
        glm::vec3 Left = glm::cross(m_target, m_up);
        glm::normalize(Left);
        Left *= STEP_SCALE;
        m_pos += Left;
        Ret = true;
        break;
    case CALLBACK_KEY_RIGHT:
    case CALLBACK_KEY_d:
        glm::vec3 Right = glm::cross(m_up, m_target);
        glm::normalize(Right);
        Right *= STEP_SCALE;
        m_pos += Right;
        Ret = true;
        break;
    case CALLBACK_KEY_PAGE_UP:
    case CALLBACK_KEY_z: // space
        m_pos.y += STEP_SCALE;
        Ret = true;
        break;
    case CALLBACK_KEY_PAGE_DOWN:
    case CALLBACK_KEY_c:
        m_pos.y -= STEP_SCALE;
        Ret = true;
        break;
    case CALLBACK_KEY_r:
        m_pos          = initPos;
        m_target       = initTarget;
        m_up           = initUp;

        Init();
        Ret = true;
        break;
    }
    return Ret;        
}


void MoveCamera::OnMouse(int x, int y)
{
    const int DeltaX = x - m_mousePos.x;
    const int DeltaY = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;

    m_AngleH += (float)DeltaX / 20.0f;
    m_AngleV += (float)DeltaY / 20.0f;

    if (DeltaX == 0) {
        if (x <= MARGIN) {
            m_OnLeftEdge = true;
        }
        else if (x >= (m_windowWidth - MARGIN2)) {
            m_OnRightEdge = true;
        }
    }
    else {
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
    }

    if (DeltaY == 0) {
        if (y <= MARGIN) {
            m_OnUpperEdge = true;
        }
        else if (y >= (m_windowHeight - MARGIN2)) {
            m_OnLowerEdge = true;
        }
    }
    else {
        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
    }

    Update();
}

void MoveCamera::Update()
{
    const glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    glm::vec3 View(1.0f, 0.0f, 0.0f);
    View = RotateVectorByQuaternion(View, m_AngleH, Vaxis);    

    glm::normalize(View);
    // Rotate the view vector by the vertical angle around the horizontal axis
    glm::vec3 Haxis = glm::cross(Vaxis, View);
    glm::normalize(Haxis);
    View = RotateVectorByQuaternion(View, m_AngleV, Haxis);

    m_target = View;
    glm::normalize(m_target);

    m_up = glm::cross(m_target, Haxis);
    glm::normalize(m_up);
}


void MoveCamera::OnRender()
{
    bool ShouldUpdate = false;

    if (m_OnLeftEdge) {
        m_AngleH -= EDGE_STEP;
        ShouldUpdate = true;
    }
    else if (m_OnRightEdge) {
        m_AngleH += EDGE_STEP;
        ShouldUpdate = true;
    } 

    if (m_OnUpperEdge) {
        if (m_AngleV > -90.0f) {
            m_AngleV -= EDGE_STEP;
            ShouldUpdate = true;
        }
    }
    else if (m_OnLowerEdge) {
        if (m_AngleV < 90.0f) {
           m_AngleV += EDGE_STEP;
           ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
}

void MoveCamera::CheckKeyboard() {
    CALLBACK_KEY list[10] = {
        CALLBACK_KEY_w, CALLBACK_KEY_s, CALLBACK_KEY_a, CALLBACK_KEY_d, CALLBACK_KEY_z, 
        CALLBACK_KEY_c, CALLBACK_KEY_UP, CALLBACK_KEY_DOWN, CALLBACK_KEY_LEFT, CALLBACK_KEY_RIGHT
    };
    for (int i = 0; i < 10; ++i){
        if (m_pCallback->GetKeyState(list[i]) == CALLBACK_KEY_STATE_PRESS){
            OnKeyboard(list[i]);
        }
    }
}

void ModelCamera::OnMouse(int x, int y)
{
    int LButton = m_pCallback->GetMouseState(CALLBACK_MOUSE_BUTTON_LEFT) 
        == CALLBACK_MOUSE_STATE_RELEASE;
    int RButton = m_pCallback->GetMouseState(CALLBACK_MOUSE_BUTTON_RIGHT)
        == CALLBACK_MOUSE_STATE_RELEASE;
    if(LButton && RButton) {
        m_mousePos.x = x;
        m_mousePos.y = y;

        return ;
    }
    const int DeltaX = x - m_mousePos.x;
    const int DeltaY = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;
    if(!LButton){
        m_longitude += (float)DeltaX / 20.0f;
        m_latitude += (float)DeltaY / 20.0f;
    } else {
        m_center += (float)DeltaX * 0.1f * m_right;
        m_center += (float)DeltaY * 0.1f * m_up;
    }

    Update();
}

void ModelCamera::Init()
{
    m_longitude = 0.0f;
    m_latitude = 0.0f;
    m_radius = 10.0f;

    m_center = glm::vec3(0.0f, 0.0f, 0.0f);

    m_mousePos.x  = m_windowWidth / 2;
    m_mousePos.y  = m_windowHeight / 2;

    Update();
}

void ModelCamera::Update()
{
    if (m_latitude > 1.5)    m_latitude = 1.5;
    if (m_latitude < -1.5)    m_latitude = -1.5;
    if (m_longitude > 2 * PI)    m_longitude -= 2 * PI;
    if (m_longitude < 0)    m_longitude += 2 * PI;
    if (m_radius < 0)    m_radius = 0;
    
    GLfloat camera_z = m_radius * cos(m_longitude) * cos(m_latitude) + m_center.z;
    GLfloat camera_x = m_radius * sin(m_longitude) * cos(m_latitude) + m_center.x;
    GLfloat camera_y = m_radius * sin(m_latitude) + m_center.y;
    m_pos = glm::vec3(camera_x, camera_y, camera_z);

    m_target = m_center - m_pos;

    m_target = glm::normalize(m_target);

    // m_up与m_target垂直
    m_right = glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f));
    m_right = glm::normalize(m_right);
    m_up = glm::cross(m_right, m_target);

    m_up = glm::normalize(m_up);
}

void ModelCamera::OnWheel(int Direction) {
    if (Direction > 0) {
        m_radius -= 1.0f;
    }
    else {
        m_radius += 1.0f;
    }
    Update();
}