#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <string>
#include <iostream>
#include <Graphics/Window.h>
#include <Core/Logger.h>
#include <vector>
#include <fstream>
#include <Entity/ComponentManager.h>
#include <Entity/Components.h>
#define LOG_MONO_DLL_CLASSES 0
namespace Fay
{
		class ScriptEngine
		{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::string& path, MonoDomain* domain);

			static void ReloadAssembly(const std::string& path);
			static void UnloadScriptDomain();

			static MonoDomain* CreateScriptDomain(const std::string& name);

			// Static method call: FayRuntime.MyScript.OnStart()
			static void InvokeRootStatic(const std::string& className, const std::string& methodName);
			static void InvokeCoreStatic(const std::string& className, const std::string& methodName);

			// Call instance method: instance.OnUpdate()
			static void InvokeRootMethod(MonoObject* instance, const std::string& className, const std::string& methodName);
			static void InvokeCoreMethod(MonoObject* instance, const std::string& className, const std::string& methodName);

			// Accessors for ECS scripts and external use
			static MonoImage* GetRootImage() { return s_rootImage; }
			static MonoImage* GetCoreImage() { return s_rootImage; }

			static MonoDomain* GetRootDomain() { return s_rootDomain; } // root
			static MonoDomain* GetScriptDomain() { return s_scriptDomain; } // hot reload

			static MonoClass* GetMonoClass(const std::string& className);
		public:
			static void createScriptTemplate(const std::string& path, uint32_t entity);
		private:
			static MonoDomain* s_rootDomain;
			static MonoDomain* s_scriptDomain;
			static std::vector<MonoDomain*> s_oldDomains;

			static MonoAssembly* s_rootAssembly;
			static MonoAssembly* s_coreAssembly;

			static MonoImage* s_rootImage;
			static MonoImage* s_coreImage;
		};
	}