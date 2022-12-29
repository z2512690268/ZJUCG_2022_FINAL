#pragma once

#include <glm/glm.hpp>
#include <vector>
class AABB
{
public:
    AABB(std::vector<glm::vec3> points){
        m_min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        m_max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto point : points) {
            m_min.x = std::min(m_min.x, point.x);
            m_min.y = std::min(m_min.y, point.y);
            m_min.z = std::min(m_min.z, point.z);
            m_max.x = std::max(m_max.x, point.x);
            m_max.y = std::max(m_max.y, point.y);
            m_max.z = std::max(m_max.z, point.z);
        }
        m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    ~AABB() {}
    void SetPos(const glm::vec3& pos) {
        m_pos = pos;
    }
    bool CheckCollision(const AABB& other) {
        if (m_max.x + m_pos.x < other.m_min.x + other.m_pos.x) return false;
        if (m_min.x + m_pos.x > other.m_max.x + other.m_pos.x) return false;
        if (m_max.y + m_pos.y < other.m_min.y + other.m_pos.y) return false;
        if (m_min.y + m_pos.y > other.m_max.y + other.m_pos.y) return false;
        if (m_max.z + m_pos.z < other.m_min.z + other.m_pos.z) return false;
        if (m_min.z + m_pos.z > other.m_max.z + other.m_pos.z) return false;
        return true;
    }
private:
    glm::vec3 m_min;
    glm::vec3 m_max;
    glm::vec3 m_pos;
};