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
		RegisterComponent<TransformComponent, SpriteComponent, CameraComponent, CubeComponent, CollisionComponent, ScriptComponent>();
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
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_CheckCollision);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_GetSpeed);

		// Input handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyPressed);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyReleased);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseDown);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseUp);
		// Scene handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_SetActive);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_GetActive);
		// Script Component
		FAY_ADD_INTERNAL_CALL(InternalCalls_ScriptComp_GetEntityId);
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
		return EditorUtils::GetSelectedEntity();
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
	bool ScriptGlue::InternalCalls_Entity_CheckCollision(MonoObject* object)
	{
		// Convert c# pointer -> EntityID
		EntityID id = GetEntityIDFromMonoObject(object);
		
		Renderable* a = nullptr;

		auto* spriteComp = ComponentManager<SpriteComponent>::Get().getComponent(id);
		if (spriteComp && spriteComp->sprite)
			a = spriteComp->sprite;

		auto* cubeComp = ComponentManager<CubeComponent>::Get().getComponent(id);
		if (!a && cubeComp && cubeComp->cube)
			a = cubeComp->cube;

		if (!a) return false;

		// Loop through every renderable in the scene
		for (Renderable* obj : EditorUtils::s_Scene->getObjects())
		{
			if (!obj) continue;

			if (obj->getId() == id) continue; // skip self

			Renderable* b = nullptr;

			// Try sprite first
			if (auto* otherSpriteComp =
				ComponentManager<SpriteComponent>::Get().getComponent(obj->getId()))
			{
				if (otherSpriteComp->sprite)
					b = otherSpriteComp->sprite;
			}

			// Try cube second
			if (!b)
			{
				if (auto* otherCubeComp =
					ComponentManager<CubeComponent>::Get().getComponent(obj->getId()))
				{
					if (otherCubeComp->cube)
						b = otherCubeComp->cube;
				}
			}

			if (!b) continue;

			bool useZ = true;

			// If both a and b are 2D (size.z == 0), ignore Z
			if (a->getSize().z == 0 && b->getSize().z == 0)
				useZ = false;

			// Perform collision check
			if (a->checkCollision(b, useZ))
				return true;
		}
		
		return false;
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

		// Check each type safely
		if (auto* sprite = entity.GetComponent<SpriteComponent>())
		{
			sprite->setPosition(Vec3(x, y, z));
			return;
		}

		if (auto* cube = entity.GetComponent<CubeComponent>())
		{
			cube->setPosition(Vec3(x, y, z));
			return;
		}

		if (auto* transform = entity.GetComponent<TransformComponent>())
		{
			transform->translation = { x, y, z };
			return;
		}
		else
		{
			FAY_LOG_WARN("Entity %u has no known position component!" << entityID);
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
	float ScriptGlue::InternalCalls_Entity_GetSpeed()
	{
		return EditorUtils::GetEntitySpeed();
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

	uint32_t ScriptGlue::InternalCalls_ScriptComp_GetEntityId()
	{
		return EditorUtils::s_currentSpriteComponent->getId();
	}
	
	void ScriptGlue::SetWindow(Window& window)
	{
		s_Window = &window;
	}

	Window& ScriptGlue::GetWindow()
	{
		if (!s_Window)
		{
			FAY_LOG_THROW_ERROR("ScriptGlue::GetWindow: Window is null!");
		}
		return *s_Window;
	}
	// Scene handling
	SceneType ScriptGlue::InternalCalls_Scene_GetActive()
	{
		return EditorUtils::GetCurrentScene();
	}
	void ScriptGlue::InternalCalls_Scene_SetActive(SceneType type)
	{
		EditorUtils::SetActiveScene(type);
	}
}