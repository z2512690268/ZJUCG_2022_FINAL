#ifndef PIPELINE_H
#define PIPELINE_H

#include "mathfunc.h"
#include "camera.h"

struct PersParam
{
    float FOV;
    float Width;
    float Height;
    float zNear;
    float zFar;
};

struct OrthoParam
{
    float right;       
    float left;        
    float bottom;      
    float top;        
    float zNear;       
    float zFar;        
};

struct Transform
{
    glm::vec3 m_scale;
    glm::vec3 m_rotation;
    glm::vec3 m_pos;

    Transform()
    {
        m_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
        m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_pos      = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::mat4x4 GetScaleMatrix() const
    {
        return glm::scale(glm::mat4x4(1.0f), m_scale);
    }

    glm::mat4x4 GetRotationMatrix() const
    {
        glm::mat4x4 RotateX = glm::rotate(glm::mat4x4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4x4 RotateY = glm::rotate(glm::mat4x4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4x4 RotateZ = glm::rotate(glm::mat4x4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return RotateZ * RotateY * RotateX;
    }

    glm::mat4x4 GetTranslationMatrix() const
    {
        return glm::transpose(glm::translate(glm::mat4x4(1.0f), m_pos));
    }

    glm::mat4x4 GetMatrix() const
    {
        // glm乘法得反过来，，，https://zhuanlan.zhihu.com/p/481867433
        return GetScaleMatrix() * GetRotationMatrix() * GetTranslationMatrix();
    }
};

class Pipeline
{
public:
    Pipeline()
    {
        worldTrans.m_scale      = glm::vec3(1.0f, 1.0f, 1.0f);
        worldTrans.m_pos   = glm::vec3(0.0f, 0.0f, 0.0f);
        worldTrans.m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Scale(float s)
    {
        Scale(s, s, s);
    }

    void Scale(const glm::vec3& scale)
    {
        Scale(scale.x, scale.y, scale.z);
    }

    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
        worldTrans.m_scale.x = ScaleX;
        worldTrans.m_scale.y = ScaleY;
        worldTrans.m_scale.z = ScaleZ;
    }

    void Translate(float x, float y, float z)
    {
        worldTrans.m_pos.x = x;
        worldTrans.m_pos.y = y;
        worldTrans.m_pos.z = z;
    }

    void Translate(const glm::vec3& Pos)
    {
        worldTrans.m_pos = Pos;
    }

    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
        worldTrans.m_rotation.x = RotateX;
        worldTrans.m_rotation.y = RotateY;
        worldTrans.m_rotation.z = RotateZ;
    }

    void Rotate(const glm::vec3& r)
    {
        Rotate(r.x, r.y, r.z);
    }

    void SetPerspectiveProj(const PersParam& p)
    {
        m_persParam = p;
    }

    void SetOrthographicProj(const OrthoParam& p)
    {
        m_orthoParam = p;
    }

    void SetCamera(const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }

    void SetCamera(const CameraBase& camera)
    {
        SetCamera(camera.GetPos(), camera.GetTarget(), camera.GetUp());
    }

    void Orient(const Transform& o)
    {
        worldTrans.m_scale      = o.m_scale;
        worldTrans.m_pos   = o.m_pos;
        worldTrans.m_rotation = o.m_rotation;
    }

    const glm::mat4x4& GetWorldTrans(){
    
        m_Wtransformation = worldTrans.GetMatrix();
        return m_Wtransformation;
    }
    const glm::mat4x4& GetViewTrans(){
        m_Vtransformation = glm::transpose(glm::lookAt(m_camera.Pos, -m_camera.Target + m_camera.Pos, m_camera.Up));
        return m_Vtransformation;
    }
    const glm::mat4x4& GetProjTrans(){
        m_ProjTransformation = glm::transpose(glm::perspectiveFovLH(glm::radians(m_persParam.FOV), m_persParam.Height, m_persParam.Width, m_persParam.zNear, m_persParam.zFar));
        // FIXED: warn: 怀疑原本代码公式出错
        // 加上下面的代码并修正test即可恢复，原理未知
        float temp = m_ProjTransformation[1][1];
        m_ProjTransformation[1][1] = m_ProjTransformation[0][0];
        m_ProjTransformation[0][0] = temp;
        return m_ProjTransformation;
    }
    const glm::mat4x4& GetOrthoTrans(){
        // TODO: debug
        m_ProjTransformation = glm::ortho(m_orthoParam.left, m_orthoParam.right, m_orthoParam.bottom, m_orthoParam.top, m_orthoParam.zNear, m_orthoParam.zFar);
        return m_ProjTransformation;
    }
    const glm::mat4x4& GetWVPTrans(){
        m_WVPtransformation = GetWorldTrans() * GetViewTrans() * GetProjTrans();
        return m_WVPtransformation;
    }
    const glm::mat4x4& GetWVOTrans(){
        m_WVOtransformation = GetWorldTrans() * GetViewTrans() * GetOrthoTrans();
        return m_WVOtransformation;
    }
private:
    Transform worldTrans;

    PersParam m_persParam;
    OrthoParam m_orthoParam;

    struct {
        glm::vec3 Pos;
        glm::vec3 Target;
        glm::vec3 Up;
    } m_camera;

    glm::mat4x4 m_WVPtransformation;
    glm::mat4x4 m_WVOtransformation;
    glm::mat4x4 m_Wtransformation;
    glm::mat4x4 m_Vtransformation;
    glm::mat4x4 m_ProjTransformation; 
    glm::mat4x4 m_OrthoTransformation; 
};


#endif  /* PIPELINE_H */
