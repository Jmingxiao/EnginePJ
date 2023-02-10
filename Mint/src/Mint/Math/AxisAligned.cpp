#include "pch.h"
#include "AxisAligned.h"


MT_NAMESPACE_BEGIN

AxisAlignedBox::AxisAlignedBox(const glm::vec3& center, const glm::vec3& half_extent) { update(center, half_extent); }

void AxisAlignedBox::merge(const glm::vec3& new_point)
{
    m_min_corner = glm::vec3(std::min(m_min_corner.x, new_point.x), std::min(m_min_corner.y, new_point.y), std::min(m_min_corner.z, new_point.z));
    m_max_corner = glm::vec3(std::max(m_max_corner.x, new_point.x), std::max(m_max_corner.y, new_point.y), std::max(m_max_corner.z, new_point.z));

    m_center = 0.5f * (m_min_corner + m_max_corner);
    m_half_extent = m_center - m_min_corner;
}

void AxisAlignedBox::update(const glm::vec3& center, const glm::vec3& half_extent)
{
    m_center = center;
    m_half_extent = half_extent;
    m_min_corner = center - half_extent;
    m_max_corner = center + half_extent;
}
MT_NAMESPACE_END