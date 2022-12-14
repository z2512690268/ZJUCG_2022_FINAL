#include "mesh.h"
#include "utils.h"
#include "mathfunc.h"

#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2
#define WVP_LOCATION 3
#define WORLD_LOCATION 7

Mesh::Mesh()
{
    m_VAO = 0;
    memset(m_Buffers, 0, sizeof(m_Buffers));
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

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> TexCoords;
    std::vector<unsigned int> Indices;

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

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Normals, TexCoords, Indices);
    }
    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    return InitBuffers(Positions, Normals, TexCoords, Indices);
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
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "pic/white.png");

            Ret = m_Textures[i]->Load();
        }
    }

    return Ret;
}

void Mesh::Render(const glm::mat4x4& WVPMatrix, const glm::mat4x4& WorldMatrix)
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

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec3> Normals;

    Positions.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Normals.reserve(NumVertices);

    for (int i = 0 ; i < NumVertices ; i++) {
        Positions.push_back(Vertices[i].m_pos);
        TexCoords.push_back(Vertices[i].m_tex);
        Normals.push_back(Vertices[i].m_normal);
    }
    m_Entries.push_back(Entry);

    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    InitBuffers(Positions, Normals, TexCoords, Indices);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    int ret = m_Entries.size() - 1;

    return ret;
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
            // 除了最后一个和第一个，都有两个三角形
            if(i != 0)
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1+1);
                // k1---k2---k1+1
            
            if(i != (_stackCount - 1))
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