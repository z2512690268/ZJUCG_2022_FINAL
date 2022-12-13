#include <stdio.h>
#include "shadow_map_fbo.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff

// ShadowMapFBO
ShadowMapFBO::ShadowMapFBO()
{
    m_fbo = 0;
    m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadowMap != 0) {
        glDeleteTextures(1, &m_shadowMap);
    }
}

bool ShadowMapFBO::Init(unsigned int Width, unsigned int Height)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ShadowMapFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}

void ShadowMapFBO::UnbindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void ShadowMapFBO::UnbindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

// ShadowMapTechnique
ShadowMapTechnique::ShadowMapTechnique()
{
}

bool ShadowMapTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }
    printf("1\n");
    if (!AddShader(GL_VERTEX_SHADER, "shader/shadow_map.vs")) {
        return false;
    }
    printf("1\n");
    if (!AddShader(GL_FRAGMENT_SHADER, "shader/shadow_map.fs")) {
        return false;
    }
    printf("1\n");
    if (!Finalize()) {
        return false;
    }
    printf("1\n");
    m_WVPLocation = GetUniformLocation("gWVP");
    m_textureLocation = GetUniformLocation("gShadowMap");

    if (m_WVPLocation == INVALID_UNIFORM_LOCATION ||
        m_textureLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    return true;
}

void ShadowMapTechnique::SetWVP(const glm::mat4& WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)&(WVP[0][0]));
}

void ShadowMapTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_textureLocation, TextureUnit);
}