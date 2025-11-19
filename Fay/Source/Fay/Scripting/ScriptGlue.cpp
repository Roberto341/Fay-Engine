#include <Scripting/ScriptGlue.h>
#include <iostream>
#include <mono/metadata/reflection.h>
#define FAY_ADD_INTERNAL_CALL(Name) mono_add_internal_call("FayRuntime.InternalCalls::" #Name, (const void*)Fay::ScriptGlue::Name)
namespace Fay
{
	Window* ScriptGlue::s_Window = nullptr;
    std::unordered_map<std::string, std::function<bool(Entity)>> ScriptGlue::s_EntityHasComponentFuncs;

	void ScriptGlue::RegisterComponents()
	{
        s_EntityHasComponentFuncs.clear();
		RegisterComponent<TransformComponent, SpriteComponent, CameraComponent, CubeComponent, CollisionComponent>();
	}
	void ScriptGlue::RegisterFunctions()
	{
		// Entity
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_GetSelected);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_HasComponent);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_SetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_GetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_SetID);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_GetID);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_SetCollision);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_GetCollision);
		// Input handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyPressed);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyReleased);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseDown);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseUp);
		// Scene handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_SetActiveScene);
		FAY_ADD_INTERNAL_CALL(InternalCalls_GetActiveScene);
	}
	bool ScriptGlue::InternalCalls_Entity_HasComponent(MonoObject* object, MonoReflectionType* componentType)
	{
		EntityID entityID = GetEntityIDFromMonoObject(object);
		if (entityID == -1)
		{
			FAY_LOG_ERROR("\033[31mEntity_HasComponent: entity ID is -1 (invalid)!\033[0m");
			return false;
		}	

		Entity entity = Entity{ entityID };

		MonoType* monoType = mono_reflection_type_get_type(componentType);
		if (!monoType)
		{
			FAY_LOG_ERROR("Entity_HasComponent: monoType is null!");
			return false;
		}

		// Replace this
		MonoClass* monoClass = mono_class_from_mono_type(monoType);
		std::string key = std::string(mono_class_get_namespace(monoClass)) + "." + mono_class_get_name(monoClass);
	
		auto it = s_EntityHasComponentFuncs.find(key);
		if (it != s_EntityHasComponentFuncs.end())
			return it->second(entity);

		FAY_LOG_ERROR("No Match Found for MonoType!");
		return false;
	}
	int ScriptGlue::InternalCalls_Entity_GetID(MonoObject* object)
	{
		EntityID entityID = GetEntityIDFromMonoObject(object);
		return entityID;
	}
	uint32_t ScriptGlue::InternalCalls_Entity_GetSelected()
	{
		return Editor::GetSelEntity();
	}
	void ScriptGlue::InternalCalls_Entity_SetID(MonoObject* object, uint32_t id)
	{
		MonoClass* klass = mono_object_get_class(object);

		MonoClassField* field = mono_class_get_field_from_name(klass, "_entityID");

		if (!field)
		{
			std::cerr << "SetEnttiyId: could not find field '_entityID'!" << std::endl;
			return;
		}
		mono_field_set_value(object, field, &id);
	}
	void ScriptGlue::InternalCalls_Entity_SetPosition(MonoObject* object, float x, float y, float z)
	{
		EntityID entityID = GetEntityIDFromMonoObject(object);
		if (entityID == -1)
		{
			FAY_LOG_ERROR("SetEntityPosition: Invalid entity (ID -1)!");
			return;
		}

		Entity entity{ entityID };

		if (entity.GetComponent<TransformComponent>())
		{
			auto& transform = *entity.GetComponent<TransformComponent>();
			transform.translation = { x, y, z };
		}
		if (entity.GetComponent<SpriteComponent>())
		{
			auto& sprite = *entity.GetComponent<SpriteComponent>();
			sprite.setPosition(Vec3(x, y, z));
		}
		if (entity.GetComponent<CubeComponent>())
		{
			auto& cube = *entity.GetComponent<CubeComponent>();
			cube.setPosition(Vec3(x, y, z));
		}
	}
	Vec3 ScriptGlue::InternalCalls_Entity_GetPosition(uint32_t entityID)
	{
		Entity entity{ entityID };

		if (entity.GetComponent<SpriteComponent>())
		{
			auto& sprite = *entity.GetComponent<SpriteComponent>();
			return sprite.getPosition();
		}
		if (entity.GetComponent<CubeComponent>())
		{
			auto& cube = *entity.GetComponent<CubeComponent>();
			return cube.getPosition();
		}
		// otherwise return fallback
		return { 0, 0, 0 };
	}
	bool ScriptGlue::InternalCalls_Entity_GetCollision(int entity)
	{
		EntityID id = static_cast<EntityID>(entity);
		if (ComponentManager<CollisionComponent>::Get().hasComponent(id))
		{
			auto* sprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
			if (sprite)
				return sprite->getCollision();
			auto* cube = ComponentManager<CubeComponent>::Get().getComponent(id);
			if (cube)
				return cube->getCollision();
		}
		// add other collision types
		return false;
	}
	
	void ScriptGlue::InternalCalls_Entity_SetCollision(int entity, bool condition)
	{
		EntityID id = static_cast<EntityID>(entity);
		auto* sprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
		auto* cube = ComponentManager<CubeComponent>::Get().getComponent(id);
		if (sprite)
		{
			//// Optionally add CollisionSpriteComponent if it's not already present
			//if (!ComponentManager<CollisionComponent>::Get().hasComponent(id))
			//{
			//	ComponentManager<CollisionComponent>::Get().addComponent(id, CollisionComponent{});
			//}
			sprite->setCollision(condition);
		}
		else if (cube)
		{
			cube->setCollision(condition);
		}
	}
	// Input handling
	bool ScriptGlue::InternalCalls_Window_KeyPressed(int keyCode)
	{
		auto& window = Fay::ScriptGlue::GetWindow();
		return window.isKeyPressed(keyCode);
	}
	bool ScriptGlue::InternalCalls_Window_KeyReleased(int keyCode)
	{
		auto& window = Fay::ScriptGlue::GetWindow();
		return window.isKeyReleased(keyCode);
	}
	bool ScriptGlue::InternalCalls_Window_MouseDown(int button)
	{
		auto& window = Fay::ScriptGlue::GetWindow();
		return window.isMouseButtonPressed(button);
	}
	bool ScriptGlue::InternalCalls_Window_MouseUp(int button)
	{
		auto& window = Fay::ScriptGlue::GetWindow();
		return window.isMouseButtonReleased(button);
	}
	void ScriptGlue::SetWindow(Window& window)
	{
		//std::cout << "[ScriptGlue] SetWindow called: " << &window << std::endl;
		s_Window = &window;
	}

	Window& ScriptGlue::GetWindow()
	{
		if (!s_Window)
		{
			std::cerr << "[ScriptGlue] ERROR: Window is not set!" << std::endl;
			throw std::runtime_error("ScriptGlue::GetWindow: Window is null!");
		}

		//std::cout << "[ScriptGlue] GetWindow called: " << s_Window << std::endl;
		return *s_Window;
	}
	// Scene handling
	SceneType ScriptGlue::InternalCalls_GetActiveScene()
	{
		return Editor::GetCurrentScene();
	}
	void ScriptGlue::InternalCalls_SetActiveScene(SceneType type)
	{
		Editor::SetActiveScene(type);
	}
}