#include "pch.h"
#include "RigidBody.h"

MT_NAMESPACE_BEGIN
RigidBodyShape::RigidBodyShape(const RigidBodyShape& res):
m_local_transform(res.m_local_transform)
{
    if (res.m_geometry->type == Gshape::box)
    {
        m_geometry = CreateRef<Box>();
    }
    else
    {
        MT_ERROR("Not supported shape type!");
    }
        
}

MT_NAMESPACE_END