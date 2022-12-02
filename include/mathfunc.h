#ifndef _MATHFUNC_H
#define _MATHFUNC_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// 绕四元数旋转glm::vec3
glm::vec3 RotateVectorByQuaternion(const glm::vec3& Vector, const float Angle, const glm::vec3& Axis);

glm::vec3 CalculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

#endif