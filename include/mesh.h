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
#include "physics/AABB.h"

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

    void Render(const glm::mat4x4& WVPMatrix, const glm::mat4x4& WorldMatrix, Texture* pInputTexture = nullptr);
        
    int InitVertexMesh(const std::vector<Vertex>& Vertices,
                        const std::vector<unsigned int>& Indices,
                        const std::string& MaterialName);
	
	// bool ExportMesh(const std::string& Filename);

	const AABB& GetAABB() const { return *m_pAABB; }

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

    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<unsigned int> m_Indices;

	AABB* m_pAABB;
};


class RectangleMesh 
{
public:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	RectangleMesh(glm::vec3 size);
private:
	glm::vec3 mRsize;
};

class PyramidMesh
{
public:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	PyramidMesh(float edgelength, float edgenum, float height);
private:
	float m_edgeLength;
	float m_edgeNum;
	float m_height;
	const float PI = 3.1415926;
};

// sector是经度， stack是纬度
class SphereMesh
{
public:
    SphereMesh(float radius=1.f, int sectorCount=36, int stackCount=18);
    ~SphereMesh() = default;

    float getRadius(void) const { return _radius; }
    int getSectorCount(void) const { return _sectorCount; }
    int getStackCount(void) const { return _stackCount; }

    void set(float r, int sectors, int stacks);
    void setRadius(float r);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
	
	void buildVertices(void);

	const std::vector<Vertex>& getVertices(void) const { return vertices; }

	const std::vector<unsigned int>& getIndices(void) const { return indices; }

private:
	const int MIN_SECTOR_COUNT = 3;
	const int MIN_STACK_COUNT = 2;

    float _radius;
    int _sectorCount;
    int _stackCount;

    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
};

class Cylinder
{
public:
    Cylinder(float baseRadius=1.f, float topRadius=1.f, float height=1.f, int sectorCount=36, int stackCount=1);
    ~Cylinder() = default;

    float getBaseRadius() const             { return _baseRadius; }
    float getTopRadius() const              { return _topRadius; }
    float getHeight() const                 { return _height; }
    int getSectorCount() const              { return _sectorCount; }
    int getStackCount() const               { return _stackCount; }
	std::vector<Vertex> getVertices(void) const { return vertices; }
	std::vector<unsigned int> getIndices(void) const { return indices; }
    void set(float baseRadius, float topRadius, float height, int sectorCount, int stackCount);
    void setBaseRadius(float radius);
    void setTopRadius(float radius);
    void setHeight(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
    void clearArray(void);
    void buildVertices(void);
    
private:

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    const int MIN_SECTORS = 3;
    const int MIN_STACKS = 1;
    float _baseRadius;
    float _topRadius;
    float _height;
    int _sectorCount;
    int _stackCount;
};


#endif  /* MESH_H */
