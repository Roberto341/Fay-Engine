#include <Scripting/ScriptGlue.h>
#include <iostream>
#include <mono/metadata/reflection.h>
#define FAY_ADD_INTERNAL_CALL(Name) mono_add_internal_call("FayRuntime.InternalCalls::" #Name, (const void*)Fay::ScriptGlue::Name)
namespace Fay
{
	Window* ScriptGlue::s_Window = nullptr;
    std::unordered_map<std::string, std::function<bool(Entity)>> ScriptGlue::s_EntityHasComponentFuncs;
	EditorUtils* ScriptGlue::s_EditorUtils = nullptr;

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
		FAY_ADD_INTERNAL_CALL(InternalCalls_Entity_HasTag);

		// Input handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyPressed);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_KeyReleased);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseDown);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Window_MouseUp);

		// Scene handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_SetActive);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_GetActive);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_CreateScene);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_SaveScene);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_LoadScene);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_SaveSceneAs);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Scene_GetChildCount);
		// Node handling
		FAY_ADD_INTERNAL_CALL(InternalCalls_Node_GetChild);
		FAY_ADD_INTERNAL_CALL(InternalCalls_Node_GetChildCount);


	}
	bool ScriptGlue::InternalCalls_Entity_HasComponent(MonoObject* object, MonoReflectionType* componentType)
	{
		EntityID entityID = GetEntityIDFromMonoObject(object);
		if (entityID == -1)
		{
			FAY_LOG_ERROR("InternalCalls_Entity_HasComponent: entity ID is -1 (invalid)!");
			return false;
		}	

		Entity entity = Entity{ entityID };

		MonoType* monoType = mono_reflection_type_get_type(componentType);
		if (!monoType)
		{
			FAY_LOG_ERROR("InternalCalls_Entity_HasComponent: monoType is null!");
			return false;
		}

		MonoClass* monoClass = mono_class_from_mono_type(monoType);
		std::string key = std::string(mono_class_get_namespace(monoClass)) + "." + mono_class_get_name(monoClass);
	
		auto it = s_EntityHasComponentFuncs.find(key);
		if (it != s_EntityHasComponentFuncs.end())
			return it->second(entity);

		FAY_LOG_ERROR("InternalCalls_Entity_HasComponent: No Match Found for MonoType!");
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
			std::cerr << "InternalCalls_Entity_SetID: could not find field '_entityID'!" << std::endl;
			return;
		}
		mono_field_set_value(object, field, &id);
	}
	bool ScriptGlue::InternalCalls_Entity_CheckCollision(MonoObject* object)
	{
		EntityID id = GetEntityIDFromMonoObject(object);

		Renderable* a = nullptr;
		Renderable* b = nullptr;

		auto* entityCollisionComponent = ComponentManager<CollisionComponent>::Get().getComponent(id);
		auto* entitySprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
		// Chcek to see if entityCollisionComponent and entitySprite are not null, and if entitySprite->sprite is not null, 
		
		// then set a to entitySprite->sprite
		if (entityCollisionComponent && entitySprite && entitySprite->sprite)
			a = entitySprite->sprite;

		// if entitySprite is not available, check for CubeComponent and set a to entityCube->cube if available 
		if (!a)
		{
			auto* entityCube = ComponentManager<CubeComponent>::Get().getComponent(id);
			if (entityCollisionComponent && entityCube && entityCube->cube)
				a = entityCube->cube;
		}

		// if it is still null, return false
		if (!a) return false;

		// Loop through every entity in the scene
		for (Renderable* obj : EditorUtils::s_Scene->getObjects())
		{
			if (!obj)
			{
				FAY_LOG_THROW_ERROR("[InternalCalls_Entity_CheckCollision]: NULL renderable in scene list");
				continue;
			}
			if (obj->getId() == id) continue; // skip self

			auto* otherEntityCollisionComponent = ComponentManager<CollisionComponent>::Get().getComponent(obj->getId());
			auto* otherEntitySprite = ComponentManager<SpriteComponent>::Get().getComponent(obj->getId());

			// Try sprite first
			if (otherEntityCollisionComponent && otherEntitySprite && otherEntitySprite->sprite)
				b = otherEntitySprite->sprite;

			// Try cube second
			if (!b)
			{
				auto* otherEntityCube = ComponentManager<CubeComponent>::Get().getComponent(obj->getId());
				if (otherEntityCollisionComponent && otherEntityCube && otherEntityCube->cube)
					b = otherEntityCube->cube;
			}

			if (!b) continue;
			
			bool useZ = true;

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
			FAY_LOG_ERROR("InternalCalls_Entity_SetPosition: Invalid entity (ID -1)!");
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
		//EditorUtils::GetCurrentSceneName();
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

	bool ScriptGlue::InternalCalls_Entity_HasTag(MonoObject* entity, MonoString* tag)
	{
		EntityID id = GetEntityIDFromMonoObject(entity);

		if (id == INVALID_ENTITY)
		{
			FAY_LOG_ERROR("[InternalCalls_Entity_HasTag] Error: Invalid Id or Null Id!");
			return false;
		}

		char* tagChars = mono_string_to_utf8(tag);
		std::string tagString(tagChars);

		mono_free(tagChars);

		auto* spriteEnt = ComponentManager<SpriteComponent>::Get().getComponent(id);

		if(spriteEnt)
			return spriteEnt->hasTag(tagString);

		auto* cubeEnt = ComponentManager<CubeComponent>::Get().getComponent(id);

		if (cubeEnt)
			return cubeEnt->hasTag(tagString);

		FAY_LOG_ERROR("[InternalCalls_Entity_HasTag] Error: Entity does not have SpriteComponent or CubeComponent!");
		
		return false;
	}
	
	void ScriptGlue::InternalCalls_Entity_SetCollision(int entity, bool condition)
	{

		// Change this to check for CollisionComponent first, then check for SpriteComponent
		// and CubeComponent, and set the collision accordingly.
		EntityID id = static_cast<EntityID>(entity);
		auto* collision = ComponentManager<CollisionComponent>::Get().getComponent(id);

		if(!collision)
		{
			FAY_LOG_ERROR("[InternalCalls_Entity_SetCollision] Error: Entity does not have CollisionComponent!");
			return;
		}    

		auto* sprite = ComponentManager<SpriteComponent>::Get().getComponent(id);
		auto* cube = ComponentManager<CubeComponent>::Get().getComponent(id);

		if (sprite && collision)
		{
			sprite->setCollision(condition);
		}
		// If sprite doesnt check for cube, if cube exists, set collision for cube

		if (cube && collision)
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
		s_Window = &window;
	}

	Window& ScriptGlue::GetWindow()
	{
		if (!s_Window)
		{
			FAY_LOG_THROW_ERROR("[ScriptGlue] GetWindow() Window is null!");
		}
		return *s_Window;
	}
	void ScriptGlue::SetEditorUtils(EditorUtils& utils)
	{
		s_EditorUtils = &utils;
	}
	EditorUtils& ScriptGlue::GetEditorUtils()
	{
		if (!s_EditorUtils)
		{
			FAY_LOG_THROW_ERROR("[ScriptGlue] GetEditorUtils() EditorUtils is null!");
		}
		return *s_EditorUtils;
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
	void ScriptGlue::InternalCalls_Scene_CreateScene(MonoString* sceneName)
	{
		char* name = mono_string_to_utf8(sceneName);
		std::string filename = std::string(name) + ".fayScene";
		std::string fullPath = "Res/Assets/Scenes/" + filename;
		if (ScriptGlue::s_EditorUtils)
			ScriptGlue::s_EditorUtils->CreateScene(fullPath);
		mono_free(name);
	}

	bool ScriptGlue::InternalCalls_Scene_SaveScene()
	{
		// Saves current scene

		if (!ScriptGlue::s_EditorUtils)
		{
			FAY_LOG_ERROR("(InternalCalls_Scene_SaceScene) Failed to save scene: EditorUtils is null!");
			return false;
		}

		ScriptGlue::s_EditorUtils->SaveScene();

		return true;	
	}
	bool ScriptGlue::InternalCalls_Scene_SaveSceneAs(MonoString* sceneName)
	{
		char* name = mono_string_to_utf8(sceneName);
		std::string filename = std::string(name);
		std::string fillpath = "Res/Assets/Scenes/" + filename;

		if (!ScriptGlue::s_EditorUtils)
		{
			FAY_LOG_ERROR("(InternalCalls_Scene_SaceSceneAs) Failed to save scene: EditorUtils is null!");
			return false;
		}

		ScriptGlue::s_EditorUtils->SaveSceneAs(fillpath);
		return true;
	}
	bool ScriptGlue::InternalCalls_Scene_LoadScene(MonoString* sceneName)
	{
		char* name = mono_string_to_utf8(sceneName);
		std::string filename = std::string(name);
		std::string fullpath = "Res/Assets/Scenes/" + filename;

		FAY_LOG_DEBUG("Loading Scene: " << fullpath);
		if (!ScriptGlue::s_EditorUtils)
		{
			FAY_LOG_ERROR("(InternalCalls_Scene_LoadScene Failed to load scene: EditorUtils is null!");
			return false;
		}
		ScriptGlue::s_EditorUtils->LoadScene(fullpath);
	}
	int ScriptGlue::InternalCalls_Scene_GetChildCount(MonoString* sceneName)
	{
		char* name = mono_string_to_utf8(sceneName);
		std::string filename = std::string(name);
		std::string fullpath = "Res/Assets/Scenes/" + filename;

		if (ScriptGlue::s_EditorUtils->GetCurrentSceneName() != ScriptGlue::s_EditorUtils->GetSceneNameFromPath(fullpath))
		{
			FAY_LOG_ERROR("InternalCalls_Scene_GetChildCount: Scene is not active! Current scene: " << ScriptGlue::s_EditorUtils->GetCurrentSceneName() << ", requested scene: " << fullpath);
			return 0;
		}

		// Get the current scene and return the number of objects in it
		return ScriptGlue::s_EditorUtils->GetScene()->getObjectCount();
	}
	uint32_t ScriptGlue::InternalCalls_Node_GetChild(MonoObject* node, uint32_t index)
	{
		NodeID nodeId = GetNodeIDFromMonoObject(node);

		if (nodeId == INVALID_NODE)
		{
			FAY_LOG_ERROR("InternalCalls_Node_GetChild: Invalid node ID!");
			return INVALID_NODE;
		}

		auto* controller = ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeId);

		if (!controller)
		{
			FAY_LOG_ERROR("InternalCalls_Node_GetChild: Node does not have a ControllerComponent!");
			return INVALID_NODE;
		}

		const auto& entities = controller->getEntities();

		if (index >= entities.size())
		{
			FAY_LOG_ERROR("InternalCalls_Node_GetChild: Index out of bounds! Node has " << entities.size() << " children, but index " << index << " was requested.");
			return INVALID_NODE;
		}

		return entities[index];
	}
	int ScriptGlue::InternalCalls_Node_GetChildCount(MonoObject* node)
	{
		NodeID nodeId = GetNodeIDFromMonoObject(node);

		if (nodeId == INVALID_NODE)
		{
			FAY_LOG_ERROR("InternalCalls_Node_GetChildCount: Invalid node ID!");
			return 0;
		}

		auto* controller = ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeId);

		if (!controller)
		{
			FAY_LOG_ERROR("InternalCalls_Node_GetChildCount: Node does not have a ControllerComponent!");
			return 0;
		}

		const auto& entities = controller->getEntities();
		return entities.size();
	}
}