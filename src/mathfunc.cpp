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