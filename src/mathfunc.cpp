#include "mathfunc.h"

// 直接相乘即为旋转，参考https://www.zhihu.com/question/37710539
glm::vec3 RotateVectorByQuaternion(const glm::vec3& Vector, const float Angle, const glm::vec3& Axis)
{
    glm::quat RotationQ = glm::angleAxis(glm::radians(Angle), Axis);
    glm::quat ConjugateQ = glm::conjugate(RotationQ);
    glm::quat VectorQ = glm::quat(0.0f, Vector.x, Vector.y, Vector.z);

    glm::quat W = RotationQ * VectorQ * ConjugateQ;
    return glm::vec3(W.x, W.y, W.z);
}