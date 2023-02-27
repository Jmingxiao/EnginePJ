#include "pch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"
#include "Mint/Core/UUID.h"
#include "Mint/Core/Input.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "Mint/Util/React3dUtil.h"

namespace Mint {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define MT_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Mint.InternalCalls::" #Name, Name)


	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		MT_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		MT_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		MT_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		MT_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		MT_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		MT_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void RigidBodyComponent_ApplyLinearImpulse(UUID entityID, glm::vec3* impulse, glm::vec3* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		MT_ASSERT(entity);

		auto& rb2d = entity.GetComponent<RigidBodyComponent>();
		rb2d.m_rigidbody->applyWorldForceAtLocalPosition(toVec3(*impulse), toVec3(*point));
		
	}

	static void RigidBodyComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec3* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		MT_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		MT_ASSERT(entity);

		auto& rb2d = entity.GetComponent<RigidBodyComponent>();
		rb2d.m_rigidbody->applyWorldForceAtCenterOfMass(toVec3(*impulse));
	}


	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
		size_t pos = typeName.find_last_of(':');
		std::string_view structName = typeName.substr(pos + 1);
		std::string managedTypename = fmt::format("Hazel.{}", structName);

		MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
		if (!managedType)
		{
			MT_ERROR("Could not find component type {}", managedTypename);
			return;
		}
		s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	
	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}


	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		MT_ADD_INTERNAL_CALL(NativeLog);
		MT_ADD_INTERNAL_CALL(NativeLog_Vector);
		MT_ADD_INTERNAL_CALL(NativeLog_VectorDot);
		
		MT_ADD_INTERNAL_CALL(GetScriptInstance);
		
		MT_ADD_INTERNAL_CALL(Entity_HasComponent);
		MT_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		
		MT_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		MT_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		
		MT_ADD_INTERNAL_CALL(RigidBodyComponent_ApplyLinearImpulse);
		MT_ADD_INTERNAL_CALL(RigidBodyComponent_ApplyLinearImpulseToCenter);
		
		MT_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}