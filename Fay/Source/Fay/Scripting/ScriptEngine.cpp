#include <Scripting/ScriptEngine.h>

namespace Fay
{
		MonoDomain* ScriptEngine::s_domain = nullptr;
		MonoAssembly* ScriptEngine::s_assembly = nullptr;
		MonoImage* ScriptEngine::s_image = nullptr;

		void ScriptEngine::Init()
		{
			mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc"); // change this to use Deps/Mono/lib
			//mono_set_dirs("Mono/lib", "Mono/etc");
			mono_config_parse(nullptr);
			s_domain = mono_jit_init("FayRuntime");
			if (!s_domain)
				std::cerr << "[Mono] Failed to initialize Mono JIT." << std::endl;
		}

		void ScriptEngine::Shutdown()
		{
			if (s_domain)
			{
				mono_jit_cleanup(s_domain);
				s_domain = nullptr;
			}
		}

		void ScriptEngine::LoadAssembly(const std::string& path)
		{
			s_assembly = mono_domain_assembly_open(s_domain, path.c_str());
			if (!s_assembly)
			{
				std::cerr << "[Mono] Failed to load assembly: " << path << std::endl;
				return;
			}
			s_image = mono_assembly_get_image(s_assembly);
			if (!s_image)
				std::cerr << "[Mono] Failed to get image from assembly." << std::endl;
		}

		void ScriptEngine::InvokeStatic(const std::string& className, const std::string& methodName)
		{
			MonoClass* klass = mono_class_from_name(s_image, "FayRuntime", className.c_str());
			if (!klass) {
				std::cerr << "[Mono] Class not found: " << className << std::endl;
				return;
			}

			MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
			if (!method) {
				std::cerr << "[Mono] Method not found: " << methodName << std::endl;
				return;
			}

			mono_runtime_invoke(method, nullptr, nullptr, nullptr);
		}
		MonoObject* ScriptEngine::CreateObject(const std::string& className)
		{
			MonoClass* klass = mono_class_from_name(s_image, "FayRuntime", className.c_str());
			if (!klass) {
				std::cerr << "[Mono] Class not found for object creation: " << className << std::endl;
				return nullptr;
			}

			MonoObject* obj = mono_object_new(s_domain, klass);
			if (!obj) {
				std::cerr << "[Mono] Failed to create object for class: " << className << std::endl;
				return nullptr;
			}

			mono_runtime_object_init(obj); // Call constructor
			return obj;
		}
		void ScriptEngine::InvokeMethod(MonoObject* instance, const std::string& className, const std::string& methodName)
		{
			if (!instance)
			{
				std::cerr << "[Mono] Attempted to invoke instance method '" << methodName
					<< "' on NULL instance of class '" << className << "'" << std::endl;
				return; // Early out to avoid the error
			}
			MonoClass* klass = mono_class_from_name(s_image, "FayRuntime", className.c_str());
			// Santity check
			if (!klass)
			{
				std::cerr << "[Mono] Class not found: " << className << std::endl;
				return;
			}
			MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
			if (!method)
			{
				std::cerr << "[Mono] Class not found: " << methodName << std::endl;
				return;
			}
			mono_runtime_invoke(method, instance, nullptr, nullptr);
		}
}