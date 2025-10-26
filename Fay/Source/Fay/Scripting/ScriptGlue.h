#pragma once
#include <Scripting/ScriptEngine.h>
#include <Entity/Components.h>
#include <Entity/Entity.h>
#include <EngineEditor/Editor.h>
#include <Entity/ComponentManager.h>
#include <mono/metadata/object.h>
#include <functional>
#include <Core/Logger.h>
namespace Fay
{
    class Window;
    enum class SceneType;
	class ScriptGlue
	{
    private:
        //static Window* s_Window;
        static Window* s_Window;
	public:
		static void RegisterComponents();
		static void RegisterFunctions();

		template<typename T>
		static void RegisterSingleComponent();

		template<typename... Component>
		static void RegisterComponent();

		//static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
        static std::unordered_map<std::string, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

        // All C# InternalCalls.cs functions
        static bool InternalCalls_Entity_HasComponent(MonoObject* object, MonoReflectionType* componentType);
        static int InternalCalls_GetEntityId(MonoObject* object);
        static uint32_t InternalCalls_GetSelectedEntity();
        static void InternalCalls_SetEntityId(MonoObject* object, uint32_t id);
        static void InternalCalls_SetEntityPosition(MonoObject* object, float x, float y, float z);
        static Vec3 InternalCalls_GetEntityPosition(uint32_t entityID);

        // Sprite 
        static Vec3 InternalCalls_Sprite_GetPosition(void* spritePtr);
        static void InternalCalls_Sprite_SetPosition(void* spritePtr, Vec3 position);

        // Cube
        static Vec3 InternalCalls_Cube_GetPosition(void* cubePtr);
        static void InternalCalls_Cube_SetPosition(void* cubePtr, Vec3 position);

        // Collsion
        static void InternalCalls_Sprite_SetCollision(void* entity, bool condition);
        static bool InternalCalls_Sprite_GetCollision(void* entity);
        static void InternalCalls_SetEntityCollision(int entityID, bool condition);
        static bool InternalCalls_GetEntityCollision(int entityID);

        // Input
        static bool InternalCalls_Window_KeyPressed(int keyCode);
        static bool InternalCalls_Window_KeyReleased(int keyCode);
        static bool InternalCalls_Window_MouseDown(int button);
        static bool InternalCalls_Window_MouseUp(int button);
        // Window stuff 
        static void SetWindow(Window& window);
        static Window& GetWindow();

        // Scene
        static SceneType InternalCalls_GetActiveScene();
        static void InternalCalls_SetActiveScene(SceneType type);
	};

    template<typename T>
    void ScriptGlue::RegisterSingleComponent()
    {
        std::string_view typeName = typeid(T).name();
        size_t pos = typeName.find_last_of(':');
        std::string_view structName = (pos == std::string_view::npos) ? typeName : typeName.substr(pos + 1);

        std::string managedTypename = "FayRuntime." + std::string(structName);

        MonoType* managedType = mono_reflection_type_from_name(const_cast<char*>(managedTypename.c_str()), ScriptEngine::GetImage());
        if (!managedType)
        {
            FAY_LOG_ERROR("Could not find component type: " << managedTypename);
            return;
        }
        MonoClass* monoClass = mono_class_from_mono_type(managedType);
        std::string key = std::string(mono_class_get_namespace(monoClass)) + "." + mono_class_get_name(monoClass);
        if (key.empty())
        {
            FAY_LOG_ERROR("Could not get type name from MonoType!");
            return;
        }
        FAY_LOG_INFO("Registered Component" << key);
        s_EntityHasComponentFuncs[key] = [](Entity entity) -> bool {
            return entity.HasComponent<T>();
            };
    }
    template<typename... Component>
    void ScriptGlue::RegisterComponent()
    {
        (RegisterSingleComponent<Component>(), ...);
    }
}