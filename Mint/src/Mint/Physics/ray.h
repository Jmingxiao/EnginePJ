#pragma once

#include<glm/glm.hpp>

MT_NAMESPACE_BEGIN
struct RayCollision
{
    void* m_node;
    glm::vec3 m_collided_point;
    float   m_ray_distance;

    RayCollision(void* node, glm::vec3 collided_point)
    {
        this->m_node = node;
        this->m_collided_point = collided_point;
        this->m_ray_distance = 0.0f;
    }

    RayCollision()
    {
        m_node = nullptr;
        m_ray_distance = FLT_MAX;
    }
};

class Ray
{
public:
    Ray(glm::vec3 start_point, glm::vec3 direction);
    ~Ray();

    glm::vec3 getStartPoint() const;
    glm::vec3 getDirection() const;

private:
    glm::vec3 m_start_point;
    glm::vec3 m_direction;
};
MT_NAMESPACE_END