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

protected:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

    std::vector<MeshEntry*> m_Entries;
    std::vector<Texture*> m_Textures;
};

class CubicMesh : public Mesh
{
public:
    CubicMesh(){
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec2(1.0f, 1.0f)),
            Vertex(glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec2(1.0f, 0.0f))
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 1, 3,
            4, 6, 5,
            5, 6, 7,
            0, 2, 4,
            4, 2, 6,
            1, 5, 3,
            3, 5, 7,
            0, 4, 1,
            1, 4, 5,
            2, 3, 6,
            6, 3, 7
        };

        m_Textures.push_back(nullptr);
        AddMeshEntry(vertices, indices, 0);

    }
};

#endif  /* MESH_H */
