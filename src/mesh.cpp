#include "mesh.h"
#include "utils.h"
#include "mathfunc.h"
#include <fstream>

#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2
#define WVP_LOCATION 3
#define WORLD_LOCATION 7

Mesh::Mesh()
{
    m_VAO = 0;
    memset(m_Buffers, 0, sizeof(m_Buffers));
    m_pAABB = nullptr;
}


Mesh::~Mesh()
{
    Clear();
}


void Mesh::Clear()
{
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        SAFE_DELETE(m_Entries[i]);
    }
    for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
        SAFE_DELETE(m_Textures[i]);
    }
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    SAFE_DELETE(m_pAABB);
}


bool Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices);
    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

    glBindVertexArray(0);

    return Ret;
}

bool Mesh::InitBuffers(const std::vector<glm::vec3>& Positions,
                    const std::vector<glm::vec3>& Normals,
                    const std::vector<glm::vec2>& TexCoords,
                    const std::vector<unsigned int>& Indices) {
    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);
    
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WVP_LOCATION + i);
        glVertexAttribPointer(WVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WVP_LOCATION + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);

    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WORLD_LOCATION + i);
        glVertexAttribPointer(WORLD_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WORLD_LOCATION + i, 1);
    }

    return glGetError() == 0;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i] = new MeshEntry();
        m_Entries[i]->MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;        
        m_Entries[i]->NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i]->BaseVertex = NumVertices;
        m_Entries[i]->BaseIndex = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i]->NumIndices;
    }

    m_Positions.clear();
    m_Normals.clear();
    m_TexCoords.clear();
    m_Indices.clear();

    // Reserve space in the vectors for the vertex attributes and indices
    m_Positions.reserve(NumVertices);
    m_Normals.reserve(NumVertices);
    m_TexCoords.reserve(NumVertices);
    m_Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, m_Positions, m_Normals, m_TexCoords, m_Indices);
    }
    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    SAFE_DELETE(m_pAABB);
    m_pAABB = new AABB(m_Positions);

    return InitBuffers(m_Positions, m_Normals, m_TexCoords, m_Indices);
}

void Mesh::InitMesh(const aiMesh* paiMesh,
                    std::vector<glm::vec3>& Positions,
                    std::vector<glm::vec3>& Normals,
                    std::vector<glm::vec2>& TexCoords,
                    std::vector<unsigned int>& Indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
    }

    // Populate the index buffer
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        // Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "pic/test.png");

            Ret = m_Textures[i]->Load();
        }
    }

    return Ret;
}

void Mesh::Render(const glm::mat4x4& WVPMatrix, const glm::mat4x4& WorldMatrix, Texture* pInputTexture)
{
    glm::mat4x4 WVP = glm::transpose(WVPMatrix);
    glm::mat4x4 World = glm::transpose(WorldMatrix);


    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4x4), &WVP, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4x4), &World, GL_DYNAMIC_DRAW);
    
    glBindVertexArray(m_VAO);
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const unsigned int MaterialIndex = m_Entries[i]->MaterialIndex;

        assert(MaterialIndex < m_Textures.size());
        
        if (m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }
        if (pInputTexture) {
            pInputTexture->Bind(GL_TEXTURE0);
        }
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 m_Entries[i]->NumIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Entries[i]->BaseIndex),
                                 m_Entries[i]->BaseVertex);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


int Mesh::InitVertexMesh(const std::vector<Vertex>& Vertices,
                    const std::vector<unsigned int>& Indices,
                    const std::string& MaterialName)
{
    // init texture
    Texture* pTexture = new Texture(GL_TEXTURE_2D, MaterialName.c_str());
    int tex_id;

    if (pTexture->Load()) {
        m_Textures.push_back(pTexture);
        tex_id = m_Textures.size() - 1;
    }
    else {
        delete pTexture;
        pTexture = new Texture(GL_TEXTURE_2D, "pic/white.png");
        pTexture->Load();
        m_Textures.push_back(pTexture);
        tex_id = m_Textures.size() - 1;
    }

    // init mesh entry
    MeshEntry* Entry = new MeshEntry();

    int NumVertices = Vertices.size();
    Entry->NumIndices = Indices.size();
    Entry->BaseVertex = 0;
    Entry->BaseIndex = 0;
    Entry->MaterialIndex = tex_id;

    m_Positions.clear();
    m_TexCoords.clear();
    m_Normals.clear();

    m_Positions.reserve(NumVertices);
    m_TexCoords.reserve(NumVertices);
    m_Normals.reserve(NumVertices);

    m_Indices = Indices;


    for (int i = 0 ; i < NumVertices ; i++) {
        m_Positions.push_back(Vertices[i].m_pos);
        m_TexCoords.push_back(Vertices[i].m_tex);
        m_Normals.push_back(Vertices[i].m_normal);
    }
    m_Entries.push_back(Entry);

    SAFE_DELETE(m_pAABB);
    m_pAABB = new AABB(m_Positions);

    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    InitBuffers(m_Positions, m_Normals, m_TexCoords, m_Indices);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    int ret = m_Entries.size() - 1;

    return ret;
}

RectangleMesh::RectangleMesh(glm::vec3 size) {
    mRsize = size;
    Indices = {
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
    Vertices = {
        Vertex(glm::vec3(-mRsize.x / 2, -mRsize.y / 2, -mRsize.z / 2), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-mRsize.x / 2, -mRsize.y / 2, +mRsize.z / 2), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-mRsize.x / 2, +mRsize.y / 2, -mRsize.z / 2), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(-mRsize.x / 2, +mRsize.y / 2, +mRsize.z / 2), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, -mRsize.y / 2, -mRsize.z / 2),  glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, -mRsize.y / 2, +mRsize.z / 2),  glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, +mRsize.y / 2, -mRsize.z / 2),  glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, +mRsize.y / 2, +mRsize.z / 2),   glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, +mRsize.y / 2, -mRsize.z / 2),  glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, +mRsize.y / 2, +mRsize.z / 2),   glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, -mRsize.y / 2, -mRsize.z / 2),  glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(+mRsize.x / 2, -mRsize.y / 2, +mRsize.z / 2),  glm::vec2(1.0f, 1.0f)),
    };
    CalcVerticesNormal(Vertices, Indices);
}

PyramidMesh::PyramidMesh(float edgelength, float edgenum, float height) {
    m_edgeLength = edgelength;
    m_edgeNum = edgenum;
    m_height = height;
    // 计算顶点投影到底面（即底面正多边形中心点）到顶点的距离
    float distance = (m_edgeLength)/2/sin(PI/m_edgeNum);
    float degree = 0;
    float texture = 0.0;
    for (int i = 0; i < m_edgeNum; i++)
    {
        Vertices.push_back(Vertex(glm::vec3(
        distance * cos(degree),
        0.0f,
        distance * sin(degree)
        ), glm::vec2(texture, 0.0f)));
        degree += 2 * PI / m_edgeNum;
        texture += (float) 1 / (m_edgeNum -1);
    }
    // 多增加一个点，此点坐标和点0相同，但是纹理不同，以此实现纹理连续，而不是0点到size-1点纹理是(0-1)
    Vertices.push_back(Vertex(glm::vec3(
        distance,
        0.0f,
        0.0f
    ), glm::vec2(texture, 0.0f)));
    Vertices.push_back(Vertex(glm::vec3(
        0.0f,
        m_height,
        0.0f), glm::vec2(0.5f, 1.0f)));
    for (int i = 0; i < m_edgeNum ; i++)
    {
        Indices.push_back(i);
        Indices.push_back(m_edgeNum+1);
        Indices.push_back(i+1);
    }
    for (int i = 2; i < m_edgeNum; i++)
    {
        Indices.push_back(0);
        Indices.push_back(i-1);
        Indices.push_back(i);
    }
    CalcVerticesNormal(Vertices, Indices);
}



// x = r * cos(phi) * cos(theta)
// y = r * cos(phi) * sin(theta)
// z = r * sin(phi)
// - pi / 2 <= phi <= pi / 2
// 0 <= theta <= 2 * pi
void SphereMesh::buildVertices(void)
{
    // clear
    vertices.clear();
    indices.clear();

    float x, y, z, xy;                              
    float nx, ny, nz, lengthInv = 1.0f / _radius;   
    float s, t;

    float sectorStep = 2 * PI / _sectorCount;
    float stackStep = PI / _stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= _stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // 从 pi/2 到 -pi/2
        xy = _radius * cosf(stackAngle);             // r * cos(phi)
        z = _radius * sinf(stackAngle);              // r * sin(phi)

        for(int j = 0; j <= _sectorCount; ++j)
        {
            Vertex v;
            sectorAngle = j * sectorStep;           // 从0到2pi

            // 顶点坐标
            x = xy * cosf(sectorAngle);            
            y = xy * sinf(sectorAngle);  
            // printf("%lf, %lf, %lf\n", x, y, z);     
            v.m_pos = glm::vec3(x, y, z);

            // 单位化法向量
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            v.m_normal = glm::vec3(nx, ny, nz);

            // 纹理坐标的范围[0, 1]
            s = (float)j / _sectorCount;
            t = (float)i / _stackCount;
            v.m_tex = glm::vec2(s, t);

            vertices.push_back(v);
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for(int i = 0; i < _stackCount; ++i)
    {
        k1 = i * (_sectorCount + 1);     // 这一纬度的开始
        k2 = k1 + _sectorCount + 1;      // 下一纬度的开始

        for(int j = 0; j < _sectorCount; ++j, ++k1, ++k2)
        {
            // 每块两个三角形
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1+1);
            // k1---k2---k1+1
            
            indices.push_back(k1+1);
            indices.push_back(k2);
            indices.push_back(k2+1);
            // k1+1---k2---k2+1
        }
    }

    return;
}

void SphereMesh::set(float r, int sectors, int stacks)
{
    setRadius(r);
    setSectorCount(sectors);
    setStackCount(stacks);
    return;
}

void SphereMesh::setRadius(float r)
{
    if(_radius != r)
        _radius = r;
    return;
}

void SphereMesh::setSectorCount(int sectorCount)
{
    if(_sectorCount != sectorCount)
        if(sectorCount < MIN_SECTOR_COUNT)
            _sectorCount = MIN_SECTOR_COUNT;
        else
            _sectorCount = sectorCount;
    return;
}

void SphereMesh::setStackCount(int stackCount)
{
    if(_stackCount != stackCount)
        if(stackCount < MIN_STACK_COUNT)
            _stackCount = MIN_STACK_COUNT;
        else
            _stackCount = stackCount;
    return;
}

SphereMesh::SphereMesh(float radius, int sectorCount, int stackCount)
{
    _radius = 0;
    _sectorCount = 0;
    _stackCount = 0;
    set(radius, sectorCount, stackCount);
    return;
}

Cylinder::Cylinder(float baseRadius, float topRadius, float height, int sectorCount, int stackCount) : _baseRadius(baseRadius), _topRadius(topRadius), _height(height), _sectorCount(sectorCount), _stackCount(stackCount)
{
    return;   
}

void Cylinder::setBaseRadius(float radius)
{
    if(radius != _baseRadius && radius >= 0)
        _baseRadius = radius;
    return;
}

void Cylinder::setTopRadius(float radius)
{
    if(radius != _topRadius && radius > 0)
        _topRadius = radius;
    return;
}

void Cylinder::setHeight(float height)
{
    if(height != _height && height > 0)
        _height = height;
    return;
}

void Cylinder::setSectorCount(int sectorCount)
{
    if(sectorCount != _sectorCount && sectorCount > 0)
        _sectorCount = sectorCount;
    return;
}

void Cylinder::setStackCount(int stackCount)
{
    if(stackCount != _stackCount && stackCount > 0)
        _stackCount = stackCount;
    return;
}

void Cylinder::set(float baseRadius, float topRadius, float height, int sectorCount, int stackCount)
{
    setHeight(height);
    setBaseRadius(baseRadius);
    setTopRadius(topRadius);
    setSectorCount(sectorCount);
    setStackCount(stackCount);
    return;
}

void Cylinder::clearArray(void)
{
    vertices.clear();
    return;
}

// 从 h/2 到 -h/2
// 使用向量的伸缩来构建
// 由方程可知任意一个圆的半径 r = (h / 2 - z)*(baseR - topR) / h + topR 
// x、y坐标放大相同倍数即r倍
void Cylinder::buildVertices(void)
{
    clearArray();

    float sectorStride = 2 * PI / _sectorCount;
    float stackStride = _height / _stackCount;

    std::vector<float> unitPos;
    // 计算单位圆坐标
    float x = 0, y = 0;
    for(unsigned int i = 0; i < _sectorCount; i++)
    {
        x = cos(i * sectorStride);
        y = sin(i * sectorStride);
        unitPos.push_back(x);
        unitPos.push_back(y);
    }
    // 计算一个圆圈的法向量
    float zAngle = atan2(_baseRadius - _topRadius, _height);
    float x0 = cos(zAngle);
    float z0 = sin(zAngle);

    std::vector<float> normals;
    for(int i = 0; i <= _sectorCount; i++)
    {
        // 将初始法向量绕着z轴旋转
        // --           --
        // |  cos   -sin |
        // |  sin    cos |
        // --           --
        // 初始y0 = 0
        float angle = i * sectorStride;
        normals.push_back(cos(angle) * x0);     // x
        normals.push_back(sin(angle) * x0);     // y
        normals.push_back(z0);                  // z
    }

    unsigned int count = unitPos.size() / 2;

    for(unsigned int i = 0; i < _stackCount; i++)
    {
        float z = _height / 2 - i * stackStride;
        float r = (_height / 2 - z) * (_baseRadius - _topRadius) / _height + _topRadius;
        float t = 1.f - (float) i / _stackCount;

        
        for(unsigned int j = 0, k = 0; j < _sectorCount; j++, k += 2)
        {
            float x = unitPos[k];
            float y = unitPos[k + 1];
            float u = (float)j / _sectorCount;
            vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec2(u, t), glm::vec3(normals[k], normals[k+1], normals[k+2])));
        }
    }

    // 圆锥底的顶点
    unsigned int baseVertexIdx = (unsigned int) vertices.size();

    float z = -_height * 0.5f;
    vertices.push_back(Vertex(glm::vec3(0, 0, z), glm::vec2(0.5f, 0.5f), glm::vec3(0, 0, -1)));
    for(int i = 0, j = 0; i < _sectorCount; i++, j += 3)
    {
        x = unitPos[j];
        y = unitPos[j+1];
        auto pos = glm::vec3(x * _baseRadius, y * _baseRadius, z);
        auto tex = glm::vec2(-x * 0.5f + 0.5f, -y * 0.5f + 0.5f);
        auto normal = glm::vec3(0, 0, -1);
        vertices.push_back(Vertex(pos, tex, normal));
    }

    // 圆锥顶的顶点
    unsigned int topVertexIdx = (unsigned int)vertices.size();

    z = _height;
    vertices.push_back(Vertex(glm::vec3(0, 0, z), glm::vec2(0.5f, 0.5f), glm::vec3(0, 0, 1)));
    for(int i = 0, j = 0; i < _sectorCount; i++, j += 3)
    {
        x = unitPos[j];
        y = unitPos[j+1];
        vertices.push_back(Vertex(
            glm::vec3(x * _topRadius, y * _topRadius, z),
            glm::vec2(x * 0.5f + 0.5f, -y * 0.5f + 0.5f),
            glm::vec3(0, 0, 1)
        ));
    }

    // 设定索引
    // 侧面的索引
    unsigned int k1, k2;
    for(int i = 0; i < _stackCount; i++)
    {
        k1 = i * (_sectorCount + 1);
        k2 = k1 + _sectorCount + 1;

        for(int j = 0; j < _sectorCount; j++, k1++, k2++)
        {
            indices.push_back(k1);
            indices.push_back(k1 + 1);
            indices.push_back(k2);

            indices.push_back(k2);
            indices.push_back(k1 + 1);
            indices.push_back(k2 + 1);
        }
    }

    auto addIdx = [&](int k1, int k2, int k3)
    {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k3);
    };

    // 底面的索引
    unsigned int baseIdx = (unsigned int)indices.size();
    for(int i = 0, k = baseVertexIdx + 1; i < _sectorCount; i++, k++)
    {
        if(i < (_sectorCount - 1))
            addIdx(baseVertexIdx, k + 1, k);
        else
            addIdx(baseVertexIdx, baseVertexIdx + 1, k);
    }

    // 顶部的索引
    unsigned int topIdx = (unsigned int)indices.size();
    for(int i = 0, k = topVertexIdx + 1; i < _sectorCount; i++, k++)
    {
        if(i < (_sectorCount - 1))
            addIdx(topVertexIdx, k, k + 1);
        else 
            addIdx(topVertexIdx, k, topVertexIdx + 1);
    }
    return;
}