#include "skybox.h"
#include "utils.h"

SkyboxTechnique::SkyboxTechnique()
{   
}

bool SkyboxTechnique::Init()
{
    if (!Technique::Init()) {
        printf("Init Failed!\n");
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "shader/skybox.vs")) {
        printf("Add Shader Failed!\n");
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "shader/skybox.fs")) {
        printf("Add Shader Failed!\n");
        return false;
    }

    if (!Finalize()) {
        printf("Finalization Failed!\n");
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_textureLocation = GetUniformLocation("gCubemapTexture");
 
    if (m_WVPLocation == INVALID_VALUE ||
        m_textureLocation == INVALID_VALUE) {
        printf("Invalid Skybox!\n");
        return false;
    }

    return true;
}


void SkyboxTechnique::SetWVP(const glm::mat4x4& WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)&WVP);    
}


void SkyboxTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_textureLocation, TextureUnit);
}

/****************************************************************************/

SkyBox::SkyBox(const CameraBase* pCamera,
               const PersParam& p)
{
    m_pCamera = pCamera;
    m_persProjInfo = p;

    m_pSkyboxTechnique = NULL;
    m_pCubemapTex = NULL;
    m_pMesh = NULL;
}


SkyBox::~SkyBox()
{
    SAFE_DELETE(m_pSkyboxTechnique);
    SAFE_DELETE(m_pCubemapTex);
    SAFE_DELETE(m_pMesh);
}


bool SkyBox::Init(const std::string& Directory,
                  const std::string& PosXFilename,
                  const std::string& NegXFilename,
                  const std::string& PosYFilename,
                  const std::string& NegYFilename,
                  const std::string& PosZFilename,
                  const std::string& NegZFilename)
{
    m_pSkyboxTechnique = new SkyboxTechnique();
    
    if (!m_pSkyboxTechnique->Init()) {
        printf("Error initializing the skybox technique\n");
        return false;
    }

    m_pSkyboxTechnique->Enable();
    m_pSkyboxTechnique->SetTextureUnit(0);

    m_pCubemapTex = new CubemapTexture(Directory,
                                       PosXFilename,
                                       NegXFilename,
                                       PosYFilename,
                                       NegYFilename,
                                       PosZFilename,
                                       NegZFilename);
    if (!m_pCubemapTex->Load()) {
        printf("Loading Skybox Failure!\n");
        return false;
    }

    m_pMesh = new Mesh();

    int ret = m_pMesh->LoadMesh("mesh/sphere.obj");
    if(!ret) {
        printf("Loading Skybox Mesh Failure!\n");
    }
    m_pSkyboxTechnique->Disable();
    return ret;
}


void SkyBox::Render()
{
    m_pSkyboxTechnique->Enable();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    Pipeline p;
    p.Scale(20.0f, 20.0f, 20.0f);
    p.Rotate(0.0f, 0.0f, 0.0f);
    p.Translate(m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);
    p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());

    p.SetPerspectiveProj(m_persProjInfo);
    m_pSkyboxTechnique->SetWVP(p.GetWVPTrans());

    m_pCubemapTex->Bind(GL_TEXTURE0);
    m_pMesh->Render(p.GetWVPTrans(), p.GetWorldTrans());
    m_pCubemapTex->Unbind(GL_TEXTURE0);

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);

    m_pSkyboxTechnique->Disable();
}
