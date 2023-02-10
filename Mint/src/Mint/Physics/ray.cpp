#include "pch.h"
#include "ray.h"

MT_NAMESPACE_BEGIN
Ray::Ray(glm::vec3 start_point, glm::vec3 direction)
    :m_start_point(start_point), m_direction(direction)
{
}

Ray::~Ray(){}

glm::vec3 Ray::getStartPoint() const
{
    return m_start_point;
}

glm::vec3 Ray::getDirection() const
{
    return m_direction;
}

MT_NAMESPACE_END