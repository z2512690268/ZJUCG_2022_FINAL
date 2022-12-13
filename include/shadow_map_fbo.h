#ifndef _SHADOW_MAP_FBO_H
#define _SHADOW_MAP_FBO_H

#include <glm/glm.hpp>
#include "technique.h"

class ShadowMapFBO
{
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

    void UnbindForWriting();

    void UnbindForReading(GLenum TextureUnit);

private:
    GLuint m_fbo;
    GLuint m_shadowMap;
};

class ShadowMapTechnique : public Technique {

public:

    ShadowMapTechnique();

    virtual bool Init();

    void SetWVP(const glm::mat4& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
private:

    GLuint m_WVPLocation;
    GLuint m_textureLocation;
};

#endif // !SHADOW_MAP_FBO_H
