#include "lighting.h"

LightingTechnique::LightingTechnique()
{   
}

bool LightingTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "shader/lighting.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "shader/lighting.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_samplerLocation = GetUniformLocation("gSampler");
    m_dirLightColorLocation = GetUniformLocation("gDirectionalLight.Color");
    m_dirLightAmbientIntensityLocation = GetUniformLocation("gDirectionalLight.AmbientIntensity");

    if (m_dirLightAmbientIntensityLocation == 0xFFFFFFFF ||
        m_WVPLocation == 0xFFFFFFFF ||
        m_samplerLocation == 0xFFFFFFFF ||
        m_dirLightColorLocation == 0xFFFFFFFF)
    {
        return false;
    }

    return true;
}

void LightingTechnique::SetWVP(const glm::mat4x4& WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)&WVP);    
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_samplerLocation, TextureUnit);
}


void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light)
{
    glUniform3f(m_dirLightColorLocation, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightAmbientIntensityLocation, Light.AmbientIntensity);
}