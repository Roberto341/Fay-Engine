#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <string>
#include <iostream>
#include <Graphics/Window.h>
namespace Fay
{
		class ScriptEngine
		{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::string& path);

			//static void InvokeMethod(const std::string& className, const std::string& methodName);
		
			// Static method call: FayRuntime.MyScript.OnStart()
			static void InvokeStatic(const std::string& className, const std::string& methodName);

			static MonoObject* CreateObject(const std::string& className);

			// Call instance method: instance.OnUpdate()
			static void InvokeMethod(MonoObject* instance, const std::string& className, const std::string& methodName);
			

			// Accessors for ECS scripts and external use
			static MonoImage* GetImage() { return s_image; }
			static MonoDomain* GetDomain() { return s_domain; }

		private:
			static MonoDomain* s_domain;
			static MonoAssembly* s_assembly;
			static MonoImage* s_image;
		};
	}