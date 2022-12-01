#ifndef _DEBUG_H
#define _DEBUG_H

#include <iostream>
#include <glm/glm.hpp>

void PrintGLMVec3(const glm::vec3& vec3){
    std::cout << "x: " << vec3.x << " y: " << vec3.y << " z: " << vec3.z << std::endl;
}

void PrintGLMVec4(const glm::vec4& vec4){
    std::cout << "x: " << vec4.x << " y: " << vec4.y << " z: " << vec4.z << " w: " << vec4.w << std::endl;
}

void PrintGLMMat4x4(const glm::mat4x4& mat4x4){
    std::cout << "m00: " << mat4x4[0][0] << " m01: " << mat4x4[0][1] << " m02: " << mat4x4[0][2] << " m03: " << mat4x4[0][3] << std::endl;
    std::cout << "m10: " << mat4x4[1][0] << " m11: " << mat4x4[1][1] << " m12: " << mat4x4[1][2] << " m13: " << mat4x4[1][3] << std::endl;
    std::cout << "m20: " << mat4x4[2][0] << " m21: " << mat4x4[2][1] << " m22: " << mat4x4[2][2] << " m23: " << mat4x4[2][3] << std::endl;
    std::cout << "m30: " << mat4x4[3][0] << " m31: " << mat4x4[3][1] << " m32: " << mat4x4[3][2] << " m33: " << mat4x4[3][3] << std::endl;
}

#endif