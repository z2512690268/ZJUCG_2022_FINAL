#include <iostream>
#include "camera.h"
#include "mathfunc.h"
#include "callback.h"

const static float STEP_SCALE = 0.1f;
const static float EDGE_STEP = 1.5f;
const static int MARGIN = 10;
const static int MARGIN2 = 30;

Camera::Camera(int WindowWidth, int WindowHeight)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos          = glm::vec3(0.0f, 0.0f, 0.0f);
    m_target       = glm::vec3(0.0f, 0.0f, 1.0f);
    m_up           = glm::vec3(0.0f, 1.0f, 0.0f);

    Init();
}

Camera::Camera(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    glm::normalize(m_target);

    m_up = Up;
    glm::normalize(m_up);

    initPos = m_pos;
    initTarget = m_target;
    initUp = m_up;

    Init();
}


void Camera::Init()
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

bool Camera::OnKeyboard(CALLBACK_KEY Key) {
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


void Camera::OnMouse(int x, int y)
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


void Camera::OnRender()
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

void Camera::Update()
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
