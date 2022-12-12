#ifndef _CAMERA_H
#define _CAMERA_H


#include "mathfunc.h"
#include "callback.h"
#include <GL/freeglut.h>

class CameraBase
{
public:
    CameraBase(int WindowWidth, int WindowHeight);

    CameraBase(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up);

    const glm::vec3& GetPos() const
    {
        return m_pos;
    }

    const glm::vec3& GetTarget() const
    {
        return m_target;
    }

    const glm::vec3& GetUp() const
    {
        return m_up;
    }

    void SetPos(const glm::vec3& Pos)
    {
        m_pos = Pos;
    }

    void SetTarget(const glm::vec3& Target)
    {
        m_target = Target;
    }

    void SetUp(const glm::vec3& Up)
    {
        m_up = Up;
    }

    void SetWindow(int WindowWidth, int WindowHeight)
    {
        m_windowWidth = WindowWidth;
        m_windowHeight = WindowHeight;
    }

    virtual void Init() {};

    virtual void Update() {};

    virtual bool OnKeyboard(CALLBACK_KEY Key) {return true;};
    
    virtual void OnMouse(int x, int y) {};

    virtual void OnRender() {};

protected:
    glm::vec3 initPos;
    glm::vec3 initTarget;
    glm::vec3 initUp;

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;

    int m_windowWidth;
    int m_windowHeight;
};

class MoveCamera : public CameraBase
{
public:

    MoveCamera(int WindowWidth, int WindowHeight) : CameraBase(WindowWidth, WindowHeight) {};

    MoveCamera(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up) :
        CameraBase(WindowWidth, WindowHeight, Pos, Target, Up) {};

    virtual bool OnKeyboard(CALLBACK_KEY Key);
    
    virtual void OnMouse(int x, int y);

    virtual void OnRender();
    
    virtual void Init();
    
    virtual void Update();
private:
    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    glm::ivec2 m_mousePos;
};

#endif	/* CAMERA_H */

