#include "pch.h"
#include "PhysicsManger.h"
#include "Mint/Util/React3dUtil.h"
#include "Mint/Scene/Entity.h"

namespace Mint {

    void PhysicsManager::Init()
    {
        rp3d::PhysicsWorld::WorldSettings settings;
        settings.defaultVelocitySolverNbIterations = 20;
        settings.isSleepingEnabled = false;
        settings.gravity = rp3d::Vector3(0.0f, -9.81f, 0.0f);

        m_world = m_physicsCommon.createPhysicsWorld(settings);
        m_world->setEventListener(&m_listener);
    }

    rp3d::CollisionBody* PhysicsManager::CreateCollisionBody(const TransformComponent& trans, const Entity& enity)
    {
      auto cb =m_world->createCollisionBody(toTrans(trans));
      m_entities[cb->getEntity().id]= enity;
      return cb;
    }

    rp3d::RigidBody* PhysicsManager::CreateRigidBody(const TransformComponent& trans, const Entity& enity)
    {
       auto rb = m_world->createRigidBody(toTrans(trans));
       m_entities[rb->getEntity().id] = enity;
       return rb;
    }

    rp3d::Collider* PhysicsManager::CreateCollider(rp3d::RigidBody* rb, Geometry* m_geometry, const glm::vec3& scale)
    {
        auto type = m_geometry->GetShape();
        switch (type)
        {
        case Mint::Gshape::none:
            MT_ERROR("Wrong colliderType")
                break;
        case Mint::Gshape::box:
        {
            Box* box = (Box*)m_geometry;
            rp3d::BoxShape* cs = m_physicsCommon.createBoxShape(toVec3(boxScale(*box, scale)));
            return rb->addCollider(cs, rp3d::Transform());

        }
        case Mint::Gshape::sphere:
        {
            Sphere* sp = (Sphere*)m_geometry;
            rp3d::SphereShape* cs = m_physicsCommon.createSphereShape(sphereScale(*sp, scale));
            return rb->addCollider(cs, rp3d::Transform());
        }
        case Mint::Gshape::capsule:
        {
            Capsule* cp = (Capsule*)m_geometry;
            auto rp3dcapsule = capsuleScale(*cp, scale);
            rp3d::CapsuleShape* cs = m_physicsCommon.createCapsuleShape(rp3dcapsule.x, rp3dcapsule.y);
            return rb->addCollider(cs, rp3d::Transform());
        }
        default:
            MT_ERROR("Can not find the colliderType")
                break;
        }
        return nullptr;
    }

    rp3d::Collider* PhysicsManager::CreateCollider(rp3d::CollisionBody* cb, Geometry* m_geometry,const glm::vec3& scale)
    {
       auto type = m_geometry->GetShape();
        switch (type)
        {
        case Mint::Gshape::none:
            MT_ERROR("Wrong colliderType")
                break;
        case Mint::Gshape::box:
        {
            Box* box = (Box*)m_geometry;
            rp3d::BoxShape* cs = m_physicsCommon.createBoxShape(toVec3(boxScale(*box, scale)));
            return cb->addCollider(cs, rp3d::Transform());
        }
        case Mint::Gshape::sphere:
        {
            Sphere* sp = (Sphere*)m_geometry;
            rp3d::SphereShape* cs = m_physicsCommon.createSphereShape(sphereScale(*sp, scale));
            return cb->addCollider(cs, rp3d::Transform());
        }
        case Mint::Gshape::capsule:
        {
            Capsule* cp = (Capsule*)m_geometry;
            auto rp3dcapsule = capsuleScale(*cp, scale);
            rp3d::CapsuleShape* cs = m_physicsCommon.createCapsuleShape(rp3dcapsule.x, rp3dcapsule.y);
            return cb->addCollider(cs, rp3d::Transform());
            
        }
        default:
            MT_ERROR("Can not find the colliderType")
                break;
        }
        return nullptr;
    }

    Entity PhysicsManager::GetEntity(rp3d::Entity e)
    {
        return m_entities[e.id];
    }

    void PhysicsManager::UpdateWorld(const float ts)
    {
        m_world->update(ts);
    }

    void PhysicsManager::UpdateContactedEntity()
    {
        for (auto& info : m_listener.contactedInfo) {
            auto& e = m_entities[info.entity];
            auto& other = m_entities[info.other];
            MT_ASSERT(e && other);
            e.OnContact(other, info.type);
        }
    }

    void PhysicsManager::UpdateTriggeredEntity()
    {
        for (auto& info : m_listener.triggerInfo) {
            auto& e = m_entities[info.entity];
            auto& other = m_entities[info.other];
            MT_ASSERT(e && other);
            e.OnTrigger(other, info.type);
        }
    }
    void PhysicsManager::Destroy()
    {
        m_physicsCommon.destroyPhysicsWorld(m_world);
        m_world = nullptr;
    }
}