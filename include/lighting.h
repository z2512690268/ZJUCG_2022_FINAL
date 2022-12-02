#ifndef LIGHTING_H
#define	LIGHTING_H

#include "technique.h"
#include <glm/glm.hpp>

class BaseLight
{
public:
    std::string Name;
    glm::vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;

    BaseLight()
    {
        Color = glm::vec3(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }      
};


class DirectionalLight : public BaseLight
{      
public:
    glm::vec3 Direction;

    DirectionalLight()
    {
        Direction = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};


struct LightAttenuation
{
    float Constant;
    float Linear;
    float Exp;
    
    LightAttenuation()
    {
        Constant = 1.0f;
        Linear = 0.0f;
        Exp = 0.0f;
    }
};


class PointLight : public BaseLight
{
public:
    glm::vec3 Position;
    LightAttenuation Attenuation;

    PointLight()
    {
        Position = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
  //  virtual void AddToATB(TwBar *bar);
};


class SpotLight : public PointLight
{
public:
    glm::vec3 Direction;
    float Cutoff;

    SpotLight()
    {
        Direction = glm::vec3(0.0f, 0.0f, 0.0f);
        Cutoff = 0.0f;
    }
    
  //  virtual void AddToATB(TwBar *bar);
};

#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_RED glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN glm::vec3(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE glm::vec3(0.0f, 0.0f, 1.0f)

class LightingTechnique : public Technique
{
public:

    LightingTechnique();

    virtual bool Init();

    void SetWVP(const glm::mat4x4& WVP);
    void SetWorldMatrix(const glm::mat4x4& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
    void SetDirectionalLight(const DirectionalLight& Light);

private:

    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
    GLuint m_samplerLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } m_dirLightLocation;
};


#endif	/* LIGHTING_TECHNIQUE_H */
