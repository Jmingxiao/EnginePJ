#pragma once
#include"Mint/Math/Transform.h"
#include "Mint/Math/AxisAligned.h"
#include "Mint/Physics/BasicShapes.h"

MT_NAMESPACE_BEGIN

enum class RigidBodyShapeType : unsigned char
{
    box,
    sphere,
    capsule,
    invalid
};

class RigidBodyShape {
public:
    Transform          m_global_transform;
    AxisAlignedBox     m_bounding_box;
    RigidBodyShapeType m_type{ RigidBodyShapeType::invalid };
    
    Transform m_local_transform;
    Ref<Geometry> m_geometry;

    RigidBodyShape() = default;
    RigidBodyShape(const RigidBodyShape& res);

};

class RigidBodyComponentRes
{
public:
    std::vector<RigidBodyShape> m_shapes;
    float                       m_inverse_mass;
    int                         m_actor_type;
};




MT_NAMESPACE_END