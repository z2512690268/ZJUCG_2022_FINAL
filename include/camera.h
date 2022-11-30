#ifndef _CAMERA_H
#define _CAMERA_H


#include "mathfunc.h"
#include "key.h"


class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up);

    bool OnKeyboard(KEY_ENUM Key);

    void OnMouse(int x, int y);

    void OnRender();

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
private:

    void Init();
    void Update();

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;

    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    glm::ivec2 m_mousePos;
};

#endif	/* CAMERA_H */

