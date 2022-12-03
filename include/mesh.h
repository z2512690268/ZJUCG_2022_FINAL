#ifndef MESH_H
#define MESH_H

#include <map>
#include <vector>
#include <GL/glew.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <glm/glm.hpp>
#include "texture.h"

struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }

    Vertex(const glm::vec3& pos, const glm::vec2& tex)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

struct MeshEntry {
    MeshEntry();

    ~MeshEntry();

    void Init(const std::vector<Vertex>& Vertices,
                const std::vector<unsigned int>& Indices);

    GLuint VB;
    GLuint IB;
    unsigned int NumIndices;
    unsigned int MaterialIndex;
};

class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render();

    int AddTexture(const std::string& Filename);

    int AddMeshEntry(const std::vector<Vertex>& Vertices,
                        const std::vector<unsigned int>& Indices,
                        unsigned int MaterialIndex);

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

    std::vector<MeshEntry*> m_Entries;
    std::vector<Texture*> m_Textures;
};


#endif  /* MESH_H */
