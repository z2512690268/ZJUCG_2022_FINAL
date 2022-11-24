#include "mathfunc.h"

glm::vec3 RotateVectorByQuaternion(const glm::vec3& Vector, const float Angle, const glm::vec3& Axis)
{
    glm::quat q = glm::angleAxis(Angle, Axis);
    glm::vec3 v = q * Vector;
    return v;
}