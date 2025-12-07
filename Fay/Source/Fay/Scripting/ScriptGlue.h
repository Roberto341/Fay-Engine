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
        static int InternalCalls_Entity_GetID(MonoObject* object);
        static void InternalCalls_Entity_SetID(MonoObject* object, uint32_t id);
        static bool InternalCalls_Entity_CheckCollision(MonoObject* object);

        static uint32_t InternalCalls_Entity_GetSelected();
        static void InternalCalls_Entity_SetPosition(MonoObject* object, float x, float y, float z);
        static Vec3 InternalCalls_Entity_GetPosition(uint32_t entityID);
        static float InternalCalls_Entity_GetSpeed();

        // Collsion
        static void InternalCalls_Entity_SetCollision(int entity, bool condition);
        static bool InternalCalls_Entity_GetCollision(int entity);

        // Input
        static bool InternalCalls_Window_KeyPressed(int keyCode);
        static bool InternalCalls_Window_KeyReleased(int keyCode);
        static bool InternalCalls_Window_MouseDown(int button);
        static bool InternalCalls_Window_MouseUp(int button);

        // Script Component
        static uint32_t InternalCalls_ScriptComp_GetEntityId();

        // Window stuff 
        static void SetWindow(Window& window);
        static Window& GetWindow();

        // Scene
        static SceneType InternalCalls_Scene_GetActive();
        static void InternalCalls_Scene_SetActive(SceneType type);
	};

    template<typename T>
    void ScriptGlue::RegisterSingleComponent()
    {
        std::string_view typeName = typeid(T).name();
        size_t pos = typeName.find_last_of(':');
        std::string_view structName = (pos == std::string_view::npos) ? typeName : typeName.substr(pos + 1);

        std::string managedTypename = "FayRuntime." + std::string(structName);

        MonoType* managedType = mono_reflection_type_from_name(const_cast<char*>(managedTypename.c_str()), ScriptEngine::GetRootImage());
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