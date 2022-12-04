#include "mathfunc.h"

glm::vec3 RotateVectorByQuaternion(const glm::vec3& Vector, const float Angle, const glm::vec3& Axis)
{
    glm::quat RotationQ = glm::angleAxis(glm::radians(Angle), Axis);
    glm::quat ConjugateQ = glm::conjugate(RotationQ);
    glm::quat VectorQ = glm::quat(0.0f, Vector.x, Vector.y, Vector.z);

    glm::quat W = RotationQ * VectorQ * ConjugateQ;
    return glm::vec3(W.x, W.y, W.z);
}

glm::vec3 CalculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
    glm::vec3 v = glm::cross(v2 - v1, v3 - v1);
    return glm::normalize(v);
}

void CalcVerticesNormal(std::vector<Vertex>& Vertices,
                    std::vector<unsigned int>& Indices)
{
    int VertexCnt = Vertices.size();
    int IndexCnt = Indices.size();

    for(int i = 0; i < IndexCnt; i += 3) {
        unsigned int Index1 = Indices[i];
        unsigned int Index2 = Indices[i + 1];
        unsigned int Index3 = Indices[i + 2];
        glm::vec3 norm = CalculateNormal(Vertices[Index1].m_pos, Vertices[Index2].m_pos, Vertices[Index3].m_pos);
        Vertices[Index1].m_normal += norm;
        Vertices[Index2].m_normal += norm;
        Vertices[Index3].m_normal += norm;
    }

    for(int i = 0; i < VertexCnt; i++) {
        glm::normalize(Vertices[i].m_normal);
    }
}