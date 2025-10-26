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
		RegisterComponent<TransformComponent, SpriteComponent, CameraComponent, CollisionSpriteComponent>();
	}
	void ScriptGlue::RegisterFunctions()
	{
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_HasComponent);
		FAY_ADD_INTERNAL_CALL(InternalCalls_SetEntityPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_GetEntityPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_SetEntityId);
		FAY_ADD_INTERNAL_CALL(InternalCalls_GetEntityId);
		FAY_ADD_INTERNAL_CALL(InternalCalls_GetSelectedEntity);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Sprite_GetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Sprite_SetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Cube_SetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Cube_GetPosition);
		FAY_ADD_INTERNAL_CALL(InternalCalls_SetEntityCollision);
		FAY_ADD_INTERNAL_CALL(InternalCalls_GetEntityCollision);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyPressed);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyReleased);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseDown);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseUp);
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
	int ScriptGlue::InternalCalls_GetEntityId(MonoObject* object)
	{
		EntityID entityID = GetEntityIDFromMonoObject(object);
		return entityID;
	}
	uint32_t ScriptGlue::InternalCalls_GetSelectedEntity()
	{
		//return Editor::GetSelEntity();
		//int selected = Editor::GetSelEntity();
		return Editor::GetSelEntity();
	}
	void ScriptGlue::InternalCalls_SetEntityId(MonoObject* object, uint32_t id)
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
	void ScriptGlue::InternalCalls_SetEntityPosition(MonoObject* object, float x, float y, float z)
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
			sprite.sprite->setPosition(Vec3(x, y, z));
		}
	}
	Vec3 ScriptGlue::InternalCalls_GetEntityPosition(uint32_t entityID)
	{
		Entity entity{ entityID };

		if (entity.GetComponent<SpriteComponent>())
		{
			auto& sprite = *entity.GetComponent<SpriteComponent>();
			return sprite.getPosition();
		}
		// otherwise return fallback
		return { 0, 0, 0 };
	}
	Vec3 ScriptGlue::InternalCalls_Sprite_GetPosition(void* spritePtr)
	{
		Sprite* sprite = reinterpret_cast<Sprite*>(spritePtr);

		return sprite->getPosition();
	}

	void ScriptGlue::InternalCalls_Sprite_SetPosition(void* spritePtr, Vec3 position)
	{
		Sprite* sprite = reinterpret_cast<Sprite*>(spritePtr);
		sprite->setPosition(position);
	}

	Vec3 ScriptGlue::InternalCalls_Cube_GetPosition(void* cubePtr)
	{
		Cube* cube = reinterpret_cast<Cube*>(cubePtr);
		return cube->getPosition();
	}
	void ScriptGlue::InternalCalls_Cube_SetPosition(void* cubePtr, Vec3 position)
	{
		Cube* cube = reinterpret_cast<Cube*>(cubePtr);
		cube->setPosition(position);
	}
	void ScriptGlue::InternalCalls_Sprite_SetCollision(void* entity, bool condition)
	{
		Sprite* sprite = reinterpret_cast<Sprite*>(entity);
		sprite->setCollision(condition);
	}
	
	bool ScriptGlue::InternalCalls_Sprite_GetCollision(void* entity)
	{
		Sprite* sprite = reinterpret_cast<Sprite*>(entity);
		return sprite->getCollision();
	}
	bool ScriptGlue::InternalCalls_GetEntityCollision(int entityID)
	{
		EntityID id = static_cast<EntityID>(entityID);
		if (ComponentManager<CollisionSpriteComponent>::Get().hasComponent(id))
		{
			auto* sprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
			if (sprite)
				return sprite->getCollision();
		}
		// add other collision types
		return false;
	}
	
	void ScriptGlue::InternalCalls_SetEntityCollision(int entityID, bool condition)
	{
		EntityID id = static_cast<EntityID>(entityID);
		auto* sprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
		if (sprite)
		{
			// Optionally add CollisionSpriteComponent if it's not already present
			if (!ComponentManager<CollisionSpriteComponent>::Get().hasComponent(id))
			{
				ComponentManager<CollisionSpriteComponent>::Get().addComponent(id, CollisionSpriteComponent{});
			}

			sprite->setCollision(condition);
		}
	}
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
	SceneType ScriptGlue::InternalCalls_GetActiveScene()
	{
		return Editor::GetCurrentScene();
	}
	void ScriptGlue::InternalCalls_SetActiveScene(SceneType type)
	{
		Editor::SetActiveScene(type);
	}
}