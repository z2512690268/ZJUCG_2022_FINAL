#ifndef SHADOWMAPFBO_H
#define	SHADOWMAPFBO_H

#include <GL/glew.h>
#include "technique.h"
#include <glm/glm.hpp>

struct PixelInfo {
    float ObjectID;
    float DrawID;
    float PrimID;
    
    PixelInfo()
    {
        ObjectID = 0.0f;
        DrawID = 0.0f;
        PrimID = 0.0f;
    }
};

class PickingTexture
{
public:
    PickingTexture();

    ~PickingTexture();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void EnableWriting();
    
    void DisableWriting();

    PixelInfo ReadPixel(unsigned int x, unsigned int y);
    
private:
    GLuint m_fbo;
    GLuint m_pickingTexture;
    GLuint m_depthTexture;
};

class PickingTechnique : public Technique
{
public:

    PickingTechnique();

    virtual bool Init();

    void SetWVP(const glm::mat4x4& WVP);

    void SetObjectIndex(unsigned int ObjectIndex);
    
private:
    
    GLuint m_WVPLocation;
    GLuint m_drawIndexLocation;
    GLuint m_objectIndexLocation;
};

#endif

