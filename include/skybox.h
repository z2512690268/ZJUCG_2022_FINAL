#ifndef SKYBOX_H
#define SKYBOX_H

#include "camera.h"
#include "technique.h"
#include "texture.h"
#include "mesh.h"
#include "pipeline.h"

class SkyboxTechnique : public Technique {
public:

    SkyboxTechnique();

    virtual bool Init();

    void SetWVP(const glm::mat4x4& WVP);
    void SetTextureUnit(unsigned int TextureUnit);

private:

    GLuint m_WVPLocation;
    GLuint m_textureLocation;
};

class SkyBox
{
public:
    SkyBox(const Camera* pCamera, const PersParam& p);

    ~SkyBox();

    bool Init(const std::string& Directory,
              const std::string& PosXFilename,
              const std::string& NegXFilename,
              const std::string& PosYFilename,
              const std::string& NegYFilename,
              const std::string& PosZFilename,
              const std::string& NegZFilename);

    void Render();

private:
    SkyboxTechnique* m_pSkyboxTechnique;
    const Camera* m_pCamera;
    CubemapTexture* m_pCubemapTex;
    Mesh* m_pMesh;
    PersParam m_persProjInfo;
};

#endif  /* SKYBOX_H */
