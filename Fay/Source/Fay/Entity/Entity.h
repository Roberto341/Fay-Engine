#pragma once
#include <cstdint>
#include <string> 
#include <mono/metadata/object.h>
#include <Entity/ComponentManager.h>
#include <Core/Logger.h>
namespace Fay
{
	using EntityID = uint32_t;
	using NodeID = uint32_t;
	constexpr EntityID INVALID_ENTITY = std::numeric_limits<uint32_t>::max();
	constexpr NodeID INVALID_NODE = std::numeric_limits<uint32_t>::max();
	/*template<typename T>
	class ComponentManager;*/
	struct Entity
	{
		EntityID id;
		std::string name;

		template<typename T>
		bool HasComponent() const
		{
			return ComponentManager<T>::Get().hasComponent(id);
		}
		template<typename T>
		T* GetComponent() const
		{
			return ComponentManager<T>::Get().getComponent(id);
		}
	};
	inline NodeID GetNodeIDFromMonoObject(MonoObject* nodeObject)
	{
		MonoClass* klass = mono_object_get_class(nodeObject);

		MonoClassField* field = mono_class_get_field_from_name(klass, "_nodeID");

		uint64_t nodeID = 0;

		mono_field_get_value(nodeObject, field, &nodeID);

		return static_cast<NodeID>(nodeID);
	}
	inline EntityID GetEntityIDFromMonoObject(MonoObject* entityObject)
	{
		MonoClass* klass = mono_object_get_class(entityObject);

		MonoClassField* field = mono_class_get_field_from_name(klass, "_entityID");

		uint64_t entityID = 0;
		mono_field_get_value(entityObject, field, &entityID);

		return static_cast<EntityID>(entityID);
	}
}