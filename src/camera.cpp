/*

	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "camera.h"
#include "mathfunc.h"

const static float STEP_SCALE = 0.2;
const static float EDGE_STEP = 1.5f;
const static int MARGIN = 10;

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

   // glutWarpPointer(m_mousePos.x, m_mousePos.y);
}


bool Camera::OnSpecialKey(int Key)
{
    bool Ret = false;

    switch (Key) {

    case GLUT_KEY_UP:
        OnKeyboard('w');
        Ret = true;
        break;

    case GLUT_KEY_DOWN:
        OnKeyboard('s');
        Ret = true;
        break;

    case GLUT_KEY_LEFT:
        OnKeyboard('a');
        Ret = true;
        break;

    case GLUT_KEY_RIGHT:
        OnKeyboard('d');
        Ret = true;
        break;
        
    case GLUT_KEY_PAGE_UP:
        OnKeyboard('z');
        Ret = true;
        break;
    
    case GLUT_KEY_PAGE_DOWN:
        OnKeyboard('c');
        Ret = true;
        break;
    
    default: 
        break;            
    }

    return Ret;
}

bool Camera::OnKeyboard(int Key) {
    bool Ret = false;
    switch (Key) {
    case 'w':
        m_pos += (m_target * STEP_SCALE);
        Ret = true;
        break;
    case 's':
        m_pos -= (m_target * STEP_SCALE);
        Ret = true;
        break;
    case 'a':
        glm::vec3 Left = glm::cross(m_target, m_up);
        glm::normalize(Left);
        Left *= STEP_SCALE;
        m_pos += Left;
        Ret = true;
        break;
    case 'd':
        glm::vec3 Right = glm::cross(m_up, m_target);
        glm::normalize(Right);
        Right *= STEP_SCALE;
        m_pos += Right;
        Ret = true;
        break;
    case 'z': // space
        m_pos.y += STEP_SCALE;
        Ret = true;
        break;
    case 'c':
        m_pos.y -= STEP_SCALE;
        Ret = true;
        break;
    case 'r':
        m_pos          = glm::vec3(0.0f, 0.0f, 0.0f);
        m_target       = glm::vec3(0.0f, 0.0f, 1.0f);
        m_up           = glm::vec3(0.0f, 1.0f, 0.0f);

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
        else if (x >= (m_windowWidth - MARGIN)) {
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
        else if (y >= (m_windowHeight - MARGIN)) {
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
