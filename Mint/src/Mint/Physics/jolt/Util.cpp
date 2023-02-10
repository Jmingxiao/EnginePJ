#include"pch.h"
#include"Util.h"


#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/StaticCompoundShape.h"

namespace Mint {

	BPLayerInterfaceImpl::BPLayerInterfaceImpl()
	{
		m_object_to_broad_phase[Layers::UNUSED1] = BroadPhaseLayers::UNUSED;
		m_object_to_broad_phase[Layers::UNUSED2] = BroadPhaseLayers::UNUSED;
		m_object_to_broad_phase[Layers::UNUSED3] = BroadPhaseLayers::UNUSED;
		m_object_to_broad_phase[Layers::UNUSED4] = BroadPhaseLayers::UNUSED;
		m_object_to_broad_phase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		m_object_to_broad_phase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		m_object_to_broad_phase[Layers::DEBRIS] = BroadPhaseLayers::DEBRIS;
		m_object_to_broad_phase[Layers::SENSOR] = BroadPhaseLayers::SENSOR;
	}

	bool ObjectCanCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2)
	{
		switch (inObject1)
		{
		case Layers::UNUSED1:
		case Layers::UNUSED2:
		case Layers::UNUSED3:
		case Layers::UNUSED4:
			return false;
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING || inObject2 == Layers::DEBRIS;
		case Layers::MOVING:
			return inObject2 == Layers::NON_MOVING || inObject2 == Layers::MOVING || inObject2 == Layers::SENSOR;
		case Layers::DEBRIS:
			return inObject2 == Layers::NON_MOVING;
		case Layers::SENSOR:
			return inObject2 == Layers::MOVING;
		default:
			MT_ASSERT(false);
			return false;
		}
	}

	bool BroadPhaseCanCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2)
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING ||
				inLayer2 == BroadPhaseLayers::SENSOR;
		case Layers::DEBRIS:
			return inLayer2 == BroadPhaseLayers::NON_MOVING;
		case Layers::SENSOR:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::UNUSED1:
		case Layers::UNUSED2:
		case Layers::UNUSED3:
			return false;
		default:
			MT_ASSERT(false);
			return false;
		}
	}

	JPH::Mat44 toMat4(const glm::mat4& m)
	{
		JPH::Vec4 cols[4];
		for (int i = 0; i < 4; i++)
		{
			cols[i] = JPH::Vec4(m[0][i], m[1][i], m[2][i], m[3][i]);
		}

		return { cols[0], cols[1], cols[2], cols[3] };
	}

	glm::mat4 toMat4(const JPH::Mat44& m)
	{
		glm::vec4 cols[4];
		for (int i = 0; i < 4; i++)
		{
			cols[i] = toVec4(m.GetColumn4(i));
		}

		return glm::transpose(glm::mat4(cols[0], cols[1], cols[2], cols[3]));
	}

	JPH::Shape* toShape(const RigidBodyShape& shape, const glm::vec3& scale)
	{
		return nullptr;
	}

}