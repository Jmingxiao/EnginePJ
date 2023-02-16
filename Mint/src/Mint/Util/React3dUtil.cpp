#include "pch.h"
#include "React3dUtil.h"
#include "Mint/Scene/Components.h"

MT_NAMESPACE_BEGIN
Matrix3x3 Mint::toMat3(const glm::mat3& m)
{   
    auto ma = glm::transpose(m);
    Matrix3x3 mat33(0);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat33[i][j] = ma[i][j];
        }
    }
    return mat33;
}

glm::mat3 Mint::toMat3(const Matrix3x3& m)
{
    return glm::mat3( toVec3(m.getColumn(0)), toVec3(m.getColumn(1)), toVec3(m.getColumn(2)));
}
rp3d::Transform Mint::toTrans(const TransformComponent& trans)
{
  return { toVec3(trans.Translation),toQuat(trans.Rotation)};
}
glm::vec3 Mint::boxScale(const Box& box, const glm::vec3& scale)
{
    return{ scale.x * box.m_half_extents.x,
            scale.y * box.m_half_extents.y,
            scale.z * box.m_half_extents.z };
}
float Mint::sphereScale(const Sphere& sphere, const glm::vec3& scale)
{
    return (scale.x + scale.y + scale.z) / 3 * sphere.m_radius;
}
glm::vec2 Mint::capsuleScale(const Capsule& capsule, const glm::vec3& scale)
{
    return { scale.z * capsule.m_half_height,
            (scale.x + scale.y) / 2 * capsule.m_radius };
}



MT_NAMESPACE_END