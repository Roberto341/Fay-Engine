#include <Scripting/ScriptEngine.h>

namespace Fay
{
		MonoDomain* ScriptEngine::s_rootDomain = nullptr;
		MonoDomain* ScriptEngine::s_scriptDomain = nullptr;

		MonoAssembly* ScriptEngine::s_rootAssembly = nullptr;
        MonoAssembly* ScriptEngine::s_coreAssembly = nullptr;

		MonoImage* ScriptEngine::s_rootImage = nullptr;
        MonoImage* ScriptEngine::s_coreImage = nullptr;

		std::vector<MonoDomain*> ScriptEngine::s_oldDomains;
        void ScriptEngine::Init()
        {
            mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");
            mono_config_parse(nullptr);

            s_rootDomain = mono_jit_init("FayRuntime"); // root domain
            if (!s_rootDomain)
                FAY_LOG_ERROR("[Mono] Failed to initialize Mono JIT.");

            s_scriptDomain = s_rootDomain; // default to root domain initially
        }
        void ScriptEngine::Shutdown()
        {
            UnloadScriptDomain(); // unload any script appdomain
            if (s_rootDomain)
            {
                mono_jit_cleanup(s_rootDomain);
                s_rootDomain = nullptr;
            }
        }
        MonoDomain* ScriptEngine::CreateScriptDomain(const std::string& name)
        {
            if (!s_rootDomain)
            {
                FAY_LOG_ERROR("[Mono] Root domain not initialized!");
                return nullptr;
            }

            MonoDomain* domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);
            if (!domain)
            {
                FAY_LOG_ERROR("[Mono] Failed to create script domain: " << name);
                return nullptr;
            }

            mono_domain_set(domain, false);
            return domain;
        }
        void ScriptEngine::UnloadScriptDomain()
        {
            if (!s_scriptDomain || s_scriptDomain == s_rootDomain)
                return; // do not unload root domain

            mono_domain_set(s_rootDomain, false); // switch back to root domain
            mono_domain_unload(s_scriptDomain);
            s_scriptDomain = nullptr;
        }
        void ScriptEngine::LoadAssembly(const std::string& path, MonoDomain* domain)
        {
            if (!domain)
                domain = s_scriptDomain ? s_scriptDomain : s_rootDomain;

            mono_domain_set(domain, false);

            MonoAssembly* assembly = mono_domain_assembly_open(domain, path.c_str());

            if (!assembly)
            {
                FAY_LOG_ERROR("[Mono] Failed to load assembly: " << path);
                return;
            }

            MonoImage* image = mono_assembly_get_image(assembly);

            if (domain == s_rootDomain)
            {
                // Runtime
                s_rootAssembly = assembly;
                s_rootImage = image;
            }
            else {
                s_coreAssembly = assembly;
                s_coreImage = image;
            }
#if LOG_MONO_DLL_CLASSES
            // Debug: print loaded classes
            const MonoTableInfo* typeDefTableConst = mono_image_get_table_info(s_rootImage, MONO_TABLE_TYPEDEF);
            if (!typeDefTableConst)
            {
                FAY_LOG_ERROR("[Mono] TypeDef table is null!");
                return;
            }

            MonoTableInfo* typeDefTable = const_cast<MonoTableInfo*>(typeDefTableConst);
            int numTypes = mono_table_info_get_rows(typeDefTable);
            FAY_LOG_DEBUG("[Mono] Classes in loaded assembly: " << numTypes);

            for (int i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefTable, i, cols, MONO_TYPEDEF_SIZE);

                const char* name = mono_metadata_string_heap(s_rootImage, cols[MONO_TYPEDEF_NAME]);
                const char* ns = mono_metadata_string_heap(s_rootImage, cols[MONO_TYPEDEF_NAMESPACE]);
                FAY_LOG_DEBUG(" - " << (ns ? ns : "") << "." << (name ? name : ""));
            }
#endif
        }

        void ScriptEngine::ReloadAssembly(const std::string& path)
        {
            UnloadScriptDomain();
            s_scriptDomain = CreateScriptDomain("ScriptDomain");
            
            LoadAssembly(path, s_scriptDomain);
        }
        void ScriptEngine::InvokeRootStatic(const std::string& className, const std::string& methodName)
        {
            if (!s_rootImage)
            {
                FAY_LOG_ERROR("[Mono] Image is NULL during InvokeRootStatic");
                return;
            }

            MonoClass* klass = mono_class_from_name(s_rootImage, "FayRuntime", className.c_str());
            if (!klass)
            {
                FAY_LOG_ERROR("[Mono] Class not found: " << className);
                return;
            }

            MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
            if (!method)
            {
                FAY_LOG_ERROR("[Mono] Method not found: " << methodName);
                return;
            }

            mono_runtime_invoke(method, nullptr, nullptr, nullptr);
        }

        void ScriptEngine::InvokeCoreStatic(const std::string& className, const std::string& methodName)
        {
            if (!s_coreImage)
            {
                FAY_LOG_ERROR("[Mono] Image is NULL during InvokeCoreStatic");
                return;
            }

            MonoClass* klass = mono_class_from_name(s_coreImage, "FayCore", className.c_str());
            if (!klass)
            {
                FAY_LOG_ERROR("[Mono] Class not found: " << className);
                return;
            }

            MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
            if (!method)
            {
                FAY_LOG_ERROR("[Mono] Method not found: " << methodName);
                return;
            }

            mono_runtime_invoke(method, nullptr, nullptr, nullptr);
        }

        void ScriptEngine::InvokeRootMethod(MonoObject* instance, const std::string& className, const std::string& methodName)
        {
            if (!instance)
            {
                FAY_LOG_ERROR("[Mono] Attempted to invoke instance method '" << methodName << "' on NULL instance of class '" << className << "'");
                return;
            }

            MonoClass* klass = mono_class_from_name(s_rootImage, "FayCore", className.c_str());
            if (!klass)
            {
                FAY_LOG_ERROR("[Mono] Class not found: " << className);
                return;
            }

            MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
            if (!method)
            {
                FAY_LOG_ERROR("[Mono] Method not found: " << methodName);
                return;
            }

            mono_runtime_invoke(method, instance, nullptr, nullptr);
        }
        void ScriptEngine::InvokeCoreMethod(MonoObject* instance, const std::string& className, const std::string& methodName)
        {
            if (!instance)
            {
                FAY_LOG_ERROR("[Mono] Attempted to invoke instance method '" << methodName << "' on NULL instance of class '" << className << "'");
                return;
            }

            MonoClass* klass = mono_class_from_name(s_coreImage, "FayCore", className.c_str());
            if (!klass)
            {
                FAY_LOG_ERROR("[Mono] Class not found: " << className);
                return;
            }

            MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
            if (!method)
            {
                FAY_LOG_ERROR("[Mono] Method not found: " << methodName);
                return;
            }

            mono_runtime_invoke(method, instance, nullptr, nullptr);
        }
        MonoClass* ScriptEngine::GetMonoClass(const std::string& className)
        {
            return mono_class_from_name(s_coreImage, "FayCore", className.c_str());
        }
        void ScriptEngine::createScriptTemplate(const std::string& path, uint32_t entity)
        {
            auto* comp = ComponentManager<ScriptComponent>::Get().getComponent(entity);
            if (comp && comp->checkId(entity))
                return; // script already exists for this entity

            std::ofstream out(path);
            if (!out.is_open())
            {
                FAY_LOG_ERROR("Failed to create script at path: " << path);
                return;
            }

            // Extract class name from file name
            std::string fileName = path.substr(path.find_last_of("/\\") + 1);
            // remove extension
            std::string className = fileName.substr(0, fileName.find_last_of('.'));

            // Write template C# class
            out << "using System;\nusing System.Collections.Generic;\nusing System.Linq;\nusing System.Text;\nusing System.Threading.Tasks;\n";
            out << "namespace FayCore\n{\n";
            out << "    public class " << className << "\n";
            out << "    {\n";
            out << "        public static void OnStart() { } \n";
            out << "        public static void OnUpdate() { } \n";
            out << "    }\n";
            out << "}\n";
            out.close();

            // Add ScriptComponent with fully qualified class name
            ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(className, entity));
        }
}