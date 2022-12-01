#include "mathfunc.h"

// 直接相乘即为旋转，参考https://www.zhihu.com/question/37710539
glm::vec3 RotateVectorByQuaternion(const glm::vec3& Vector, const float Angle, const glm::vec3& Axis)
{
    glm::quat q = glm::angleAxis(Angle, Axis);
    glm::vec3 v = glm::mat4_cast(q) * glm::vec4(Vector, 1.0f);
    return v;
}