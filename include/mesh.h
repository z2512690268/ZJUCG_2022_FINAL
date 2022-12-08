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
#include "utils.h"

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3    
#define WVP_MAT_VB   4
#define WORLD_MAT_VB 5

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
    MeshEntry(){
        NumIndices = 0;
        BaseVertex = 0;
        BaseIndex = 0;
        MaterialIndex = INVALID_VALUE;
    };

    unsigned int NumIndices;
	unsigned int BaseVertex;
    unsigned int BaseIndex;
    unsigned int MaterialIndex;
};

class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render(const glm::mat4x4& WVPMatrix, const glm::mat4x4& WorldMatrix);
        
    int InitVertexMesh(const std::vector<Vertex>& Vertices,
                        const std::vector<unsigned int>& Indices,
                        const std::string& MaterialName);

protected:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(const aiMesh* paiMesh,
                    std::vector<glm::vec3>& Positions,
                    std::vector<glm::vec3>& Normals,
                    std::vector<glm::vec2>& TexCoords,
                    std::vector<unsigned int>& Indices);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    bool InitBuffers(const std::vector<glm::vec3>& Positions,
                        const std::vector<glm::vec3>& Normals,
                        const std::vector<glm::vec2>& TexCoords,
                        const std::vector<unsigned int>& Indices);  
    void Clear();

    GLuint m_VAO;
    GLuint m_Buffers[6];
    std::vector<MeshEntry*> m_Entries;
    std::vector<Texture*> m_Textures;
};

#endif  /* MESH_H */
