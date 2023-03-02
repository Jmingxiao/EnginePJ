#pragma once
#include "Mint/Scene/Components.h"
#include "Mint/Physics/PhysicsManger.h"

namespace Mint {

	class Entity;

	enum OnCollisionType {
		Enter = 0,Stay,Exit
	};

	struct HitInfo {
		OnCollisionType type{};
		uint32_t entity{};
		uint32_t other{};
	};
	class PhysicsListener : public rp3d::EventListener {

	public:
		virtual void onContact(const rp3d::CollisionCallback::CallbackData& callbackData)override {
			contactedInfo.resize(callbackData.getNbContactPairs());
			for (uint32_t p = 0; p < callbackData.getNbContactPairs(); p++) {
				CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
				contactedInfo[p] = { (OnCollisionType)contactPair.getEventType(),contactPair.getBody1()->getEntity().id,
					contactPair.getBody2()->getEntity().id};
			}
		}
		virtual void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override {

			triggerInfo.resize(callbackData.getNbOverlappingPairs());
			for (uint32_t p = 0; p < callbackData.getNbOverlappingPairs(); p++) {
				auto contactPair = callbackData.getOverlappingPair(p);
				contactedInfo[p] = { (OnCollisionType)contactPair.getEventType(),contactPair.getBody1()->getEntity().id,
					contactPair.getBody2()->getEntity().id};
			}

		}
		std::vector<HitInfo> contactedInfo;
		std::vector<HitInfo> triggerInfo;
	};

	class PhysicsManager {

	public:
		void Init();
		rp3d::CollisionBody* CreateCollisionBody(const TransformComponent& trans,const Entity& enity);
		rp3d::RigidBody* CreateRigidBody(const TransformComponent& trans, const Entity& enity);
		rp3d::Collider* CreateCollider(rp3d::RigidBody* rb, Geometry* m_geometry, const glm::vec3& scale);
		rp3d::Collider* CreateCollider(rp3d::CollisionBody* cb, Geometry* m_geometry, const glm::vec3& scale);

	public:
		rp3d::PhysicsWorld* GetWorld() { return m_world; }
		rp3d::PhysicsCommon& GetManager() { return m_physicsCommon;}
		Entity GetEntity(rp3d::Entity);
		void UpdateWorld(const float);
		void UpdateContactedEntity();
		void UpdateTriggeredEntity();
		void Destroy();
	private:
		rp3d::PhysicsCommon m_physicsCommon;
		rp3d::PhysicsWorld* m_world;
		PhysicsListener m_listener;
		std::unordered_map<uint32_t, Entity> m_entities;
	};
}