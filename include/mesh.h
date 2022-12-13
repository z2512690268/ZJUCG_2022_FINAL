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


class RectangleMesh 
{
public:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices = {
			0, 1, 2,
			2, 1, 3,
			4, 6, 5,
			5, 6, 7,
			0, 2, 4,
			4, 2, 6,
			1, 5, 3,
			3, 5, 7,
			8, 2, 9,
			9, 2, 3,
			0, 10, 1,
			1, 10, 11,
	};
	RectangleMesh() {
		mRsize = glm::vec3(1.0,1.0,1.0);
		mRpos = glm::vec3(0.0, 0.0, 0.0);
		Vertices = {
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2), glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2), glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2),   glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2),   glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2),  glm::vec2(1.0f, 1.0f)),
		};
	};

	RectangleMesh(glm::vec3 size, glm::vec3 pos) {
		mRsize = size;
		mRpos = pos;
		Vertices = {
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2), glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x - mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2), glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2),   glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y + mRsize.z / 2, mRpos.z + mRsize.y / 2),   glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z - mRsize.y / 2),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(mRpos.x + mRsize.x / 2, mRpos.y - mRsize.z / 2, mRpos.z + mRsize.y / 2),  glm::vec2(1.0f, 1.0f)),
		};
	}
private:
	glm::vec3 mRsize;
	glm::vec3 mRpos;
};

class PyramidMesh
{
public:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	PyramidMesh(glm::vec3 size,glm::vec3 pos) {
		mPsize = size;
		mPpos = pos;
		// 计算顶点投影到底面（即底面正多边形中心点）到顶点的距离
		float distance = (mPsize.x)/2/sin(PI/mPsize.y);
		float degree = 0;
		float texture = 0.0;
		for (int i = 0; i < mPsize.y; i++)
		{
			Vertices.push_back(Vertex(glm::vec3(
			mPpos.x + distance * cos(degree),
			mPpos.y ,
			mPpos.z + distance * sin(degree)
			), glm::vec2(texture, 0.0f)));
			degree += 2 * PI / mPsize.y;
			texture += (float) 1 / (mPsize.y -1);
		}
		// 多增加一个点，此点坐标和点0相同，但是纹理不同，以此实现纹理连续，而不是0点到size-1点纹理是(0-1)
		Vertices.push_back(Vertex(glm::vec3(
			mPpos.x + distance,
			mPpos.y ,
			mPpos.z 
		), glm::vec2(texture, 0.0f)));
		Vertices.push_back(Vertex(glm::vec3(
			mPpos.x,
			mPpos.y + mPsize.z,
			mPpos.z
		), glm::vec2(0.5f, 1.0f)));
		for (int i = 0; i < mPsize.y ; i++)
		{
			Indices.push_back(i);
			Indices.push_back(mPsize.y+1);
			Indices.push_back(i+1);
		}
		for (int i = 2; i < mPsize.y; i++)
		{
			Indices.push_back(0);
			Indices.push_back(i-1);
			Indices.push_back(i);
		}
		
	}
private:
	glm::vec3 mPpos;
	glm::vec3 mPsize;
	const float PI = 3.1415926;
};

#endif  /* MESH_H */
