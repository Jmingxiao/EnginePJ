#pragma once
#include <limits>
#include "glm/glm.hpp"

MT_NAMESPACE_BEGIN
class AxisAlignedBox
{
public:
	AxisAlignedBox() {}
	AxisAlignedBox(const glm::vec3& center, const glm::vec3& half_extent);

    void merge(const glm::vec3& new_point);
    void update(const glm::vec3& center, const glm::vec3& half_extent);

    const glm::vec3& getCenter() const { return m_center; }
    const glm::vec3& getHalfExtent() const { return m_half_extent; }
    const glm::vec3& getMinCorner() const { return m_min_corner; }
    const glm::vec3& getMaxCorner() const { return m_max_corner; }

private:
    glm::vec3 m_center{};
    glm::vec3 m_half_extent{};

    glm::vec3 m_min_corner{
        std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    glm::vec3 m_max_corner{
        -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
};
MT_NAMESPACE_END