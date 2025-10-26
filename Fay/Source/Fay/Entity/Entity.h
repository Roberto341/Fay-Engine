#pragma once
#include <cstdint>
#include <string> 
#include <mono/metadata/object.h>
#include <Entity/ComponentManager.h>
namespace Fay
{
	using EntityID = uint32_t;
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
	inline EntityID GetEntityIDFromMonoObject(MonoObject* entityObject)
	{
		MonoClass* klass = mono_object_get_class(entityObject);

		MonoClassField* field = mono_class_get_field_from_name(klass, "_entityID");

		uint64_t entityID = 0;
		mono_field_get_value(entityObject, field, &entityID);

		return static_cast<EntityID>(entityID);
	}
}