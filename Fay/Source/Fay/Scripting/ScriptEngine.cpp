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
        static std::filesystem::path s_exeDir;
        void ScriptEngine::Init(const char* argv0)
        {
            if (!argv0)
                throw std::runtime_error("argv[0] is null");

            s_exeDir = std::filesystem::absolute(argv0).parent_path();
            mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");
            mono_config_parse(nullptr);
            
            // Root domain - stays alive for the entire editor lifetime.

            s_rootDomain = mono_jit_init("FayRuntime");

            if (!s_rootDomain)
            {
                FAY_LOG_ERROR("[Mono] Failed to initialize Mono JIT.");
                return;
            }
            // FayCore gets its own domain when Play is pressed
            s_scriptDomain = nullptr; 
        }
        const std::filesystem::path& ScriptEngine::ExeDir()
        {
            return s_exeDir;
        }
        std::string ScriptEngine::GetRuntimeDll()
        {
            namespace fs = std::filesystem;

            if (s_exeDir.empty())
                FAY_LOG_THROW_ERROR("ScriptEngine not initialized");

            // EXE is in Fay/Bin -> go up to Fay root
            fs::path fayRoot = s_exeDir
                .parent_path()
                .parent_path()
                .parent_path();

            fs::path dllPath = fayRoot / "Fay/Source/Fay/Scripting/FayRuntime/bin/Debug/net48/FayRuntime.dll"; // Change to release after completion

            if (!fs::exists(dllPath))
                FAY_LOG_THROW_ERROR("FayRuntime.dll not found at" + dllPath.string());

            return dllPath.string();
        }
        std::string ScriptEngine::GetCoreDll()
        {
            namespace fs = std::filesystem;

            if (s_exeDir.empty())
                FAY_LOG_THROW_ERROR("ScriptEngine not initialized");

            // EXE is in Fay/Bin -> go up to Fay root
            fs::path fayRoot = s_exeDir
                .parent_path()
                .parent_path()
                .parent_path();

            fs::path dllPath = fayRoot / "Fay/Source/Fay/Scripting/FayCore/bin/Any CPU/Debug/net48/FayCore.dll"; // Change to release after completion

            if (!fs::exists(dllPath))
                FAY_LOG_THROW_ERROR("FayCore.dll not found at" + dllPath.string());



            return dllPath.string();
        }
        std::string ScriptEngine::GetCoreCsProj()
        {
            namespace fs = std::filesystem;

            if (s_exeDir.empty())
                FAY_LOG_THROW_ERROR("ScriptEngine not initialized");

            fs::path fayRoot = s_exeDir
                .parent_path()
                .parent_path()
                .parent_path();
            
            fs::path csPath = fayRoot / "Fay/Source/Fay/Scripting/FayCore/FayCore.csproj";
          
            if (!fs::exists(csPath))
                FAY_LOG_THROW_ERROR("FayCore.csproj not found at" + csPath.string());


            return csPath.string();
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
            // Do not unload root domain, only script domain
            if (!s_scriptDomain || s_scriptDomain == s_rootDomain)
                return; 

            // Switch back to root
            mono_domain_set(s_rootDomain, false);
            mono_domain_unload(s_scriptDomain);
            s_scriptDomain = nullptr;
        }
        void ScriptEngine::DestroyScriptDomain()
        {
            if (!s_scriptDomain || s_scriptDomain == s_rootDomain)
                return;

            FAY_LOG_INFO("[Mono] Unloading FayCore script domain...");

            UnloadScriptDomain();

            s_scriptDomain = nullptr;
            s_coreAssembly = nullptr;
            s_coreImage = nullptr;

            FAY_LOG_INFO("[Mono] FayCore script domain unloaded.");
        }
        void ScriptEngine::LoadAssembly(const std::string& path, MonoDomain* domain)
        {
            if (!domain)
                domain = s_scriptDomain ? s_scriptDomain : s_rootDomain;

            mono_domain_set(domain, false);

            MonoAssembly* assembly =
                mono_domain_assembly_open(domain, path.c_str());

            if (!assembly)
            {
                FAY_LOG_ERROR("[Mono] Failed to load assembly: " << path);
                return;
            }

            MonoImage* image = mono_assembly_get_image(assembly);

            if (domain == s_rootDomain)
            {
                s_rootAssembly = assembly;
                s_rootImage = image;
            }
            else
            {
                s_coreAssembly = assembly;
                s_coreImage = image;
            }

#if LOG_MONO_DLL_CLASSES

            // Always inspect the assembly we JUST loaded.
            const MonoTableInfo* typeDefTable =
                mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

            if (!typeDefTable)
            {
                FAY_LOG_ERROR("[Mono] TypeDef table is null!");
                return;
            }

            int numTypes = mono_table_info_get_rows(typeDefTable);

            FAY_LOG_DEBUG(
                "[Mono] Classes in loaded assembly: "
                << numTypes
            );

            for (int i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];

                mono_metadata_decode_row(
                    typeDefTable,
                    i,
                    cols,
                    MONO_TYPEDEF_SIZE
                );

                const char* name =
                    mono_metadata_string_heap(
                        image,
                        cols[MONO_TYPEDEF_NAME]
                    );

                const char* ns =
                    mono_metadata_string_heap(
                        image,
                        cols[MONO_TYPEDEF_NAMESPACE]
                    );

                FAY_LOG_DEBUG(
                    " - "
                    << (ns ? ns : "")
                    << "."
                    << (name ? name : "")
                );
            }

#endif
        }

        bool ScriptEngine::BuildAndLoadCoreAssembly()
        {
            if (!s_scriptDomain)
            {
                s_scriptDomain = CreateScriptDomain("FayCore");

                if (!s_scriptDomain)
                {
                    FAY_LOG_THROW_ERROR("[Mono] Failed to create FayCore script domain.");
                    return false;
                }
            }

            // ======================================
            // BUILD ONLY IF SCRIPTS CHANGED
            // ======================================

            if (ScriptsNeedRebuild())
            {
                FAY_LOG_INFO("[Mono] Scripts changed. Building FayCore...");

                // Path to the FayCore project
                const std::string projectPath = GetCoreCsProj();

                // Path where FayCore.dll is produced
                const std::string coreDllPath = GetCoreDll();

                // MSBuild path
                const std::string msbuildPath =
                    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe";

                std::string systemCommand =
                    "cmd.exe /C \"\"" + msbuildPath + "\" \"" +
                    projectPath +
                    "\" /t:Rebuild /p:Configuration=Debug /p:Platform=\"Any CPU\"\"";

                FAY_LOG_INFO("[Mono] System command:");
                FAY_LOG_INFO(systemCommand.c_str());

                int result = std::system(systemCommand.c_str());

                if (result != 0)
                {
                    FAY_LOG_THROW_ERROR(
                        "[Mono] Failed to build FayCore. MSBuild returned: " +
                        std::to_string(result)
                    );

                    return false;
                }

                FAY_LOG_INFO("[Mono] FayCore build succeeded.");
            }
            else
            {
                FAY_LOG_INFO(
                    "[Mono] No script changes detected. Skipping build."
                );
            }

            // ======================================
            // VERIFY DLL
            // ======================================

            const std::string coreDllPath = GetCoreDll();

            if (!std::filesystem::exists(coreDllPath))
            {
                FAY_LOG_THROW_ERROR(
                    "[Mono] FayCore.dll was not found: " + coreDllPath
                );

                return false;
            }

            // ======================================
            // LOAD FAYCORE INTO SCRIPT DOMAIN
            // ======================================

            if (!s_scriptDomain)
            {
                FAY_LOG_THROW_ERROR("[Mono] Script domain is null.");
                return false;
            }

            FAY_LOG_INFO("[Mono] Loading FayCore from:");
            FAY_LOG_INFO(coreDllPath.c_str());

            LoadAssembly(coreDllPath, s_scriptDomain);

            // ======================================
            // VERIFY ASSEMBLY
            // ======================================

            if (!s_coreAssembly || !s_coreImage)
            {
                FAY_LOG_THROW_ERROR(
                    "[Mono] FayCore assembly failed to load."
                );

                return false;
            }

            FAY_LOG_INFO("[Mono] FayCore loaded successfully.");

            return true;
        }

        bool ScriptEngine::ScriptsNeedRebuild()
        {
            namespace fs = std::filesystem;

            if (s_exeDir.empty())
                FAY_LOG_THROW_ERROR("ScriptEngine not initialized");

            // EXE is in Fay/Bin -> go up to Fay root

            fs::path fayRoot = s_exeDir
                .parent_path()
                .parent_path()
                .parent_path();

            fs::path scriptsPath = fayRoot / "Fay/Res/Assets/Scripts";

            fs::path dllPath = GetCoreDll();

            // No DLL means we have to build it.
            if (!fs::exists(dllPath))
                return true;

            // Checl all C# scripts, including subdirs.
            for (const auto& entry : fs::recursive_directory_iterator(scriptsPath))
            {
                if (!entry.is_regular_file())
                    continue;

                if (entry.path().extension() != ".cs")
                    continue;

                if (fs::last_write_time(entry.path()) > fs::last_write_time(dllPath))
                {
                    return true;
                }
            }
            return false;
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

            std::string fileName = path.substr(path.find_last_of("/\\") + 1);
            std::string className = fileName.substr(0, fileName.find_last_of('.'));

            if (comp)
            {
                for (auto& s : comp->scripts)
                {
                    if (s.className == className)
                    {
                        FAY_LOG_WARN("Script " << className << " already attached to entity " << entity);
                        return; // script already exists
                    }
                }
            }
           
            createTemplate(path, className);
              
            // If no ScriptComponent exists, create it
            if (!comp)
            {
                ScriptComponent newComp(entity);
                newComp.scripts.emplace_back(className);
                ComponentManager<ScriptComponent>::Get().addComponent(entity, newComp);
                comp = ComponentManager<ScriptComponent>::Get().getComponent(entity);
            }
            else {
                // Otherwise, just add the new script to the existing component
                comp->scripts.emplace_back(className);
            }

            // Auto-inialize the new script
			// Disabled for right now as it does not need to auto initialize on creation, only when the scene is running
            /*
            auto& newScript = comp->scripts.back();
            if (!newScript.hasStarted)
            {
                ScriptEngine::InvokeCoreStatic(newScript.className, "OnStart");
                newScript.hasStarted = true;
            }
            */
            FAY_LOG_INFO("Script " << className << " created and attatched to entity " << entity);
        }
        void ScriptEngine::createScriptTemplateNode(const std::string& path, uint32_t node)
        {
            auto* comp = ComponentManager<ScriptComponent>::Get().getNodeComponent(node);

            std::string fileName = path.substr(path.find_last_of("/\\") + 1);
            std::string className = fileName.substr(0, fileName.find_last_of('.'));

            if (comp)
            {
                for (auto& s : comp->scripts)
                {
                    if (s.className == className)
                    {
                        FAY_LOG_WARN("Script " << className << " already attached to node " << node);
                        return;// ignore
                    }
                }
            }

            std::ofstream out(path);
            if (!out.is_open())
            {
                FAY_LOG_ERROR("Failed to create script at path" << path);
                return;
            }
            // Write template
            createTemplate(path, className);

            // If no script component exists, create it
            if (!comp)
            {
                ScriptComponent newComp(node);
                newComp.scripts.emplace_back(className);
                ComponentManager<ScriptComponent>::Get().addNodeComponent(node, newComp);
                comp = ComponentManager<ScriptComponent>::Get().getNodeComponent(node);
            }
            else
            {
                // Otherwise, just add the new script to the existing component
                comp->scripts.emplace_back(className);
            }

            FAY_LOG_INFO("Script " << className << " created and attached to node " << node);

        }
        void ScriptEngine::createTemplate(const std::string& path, const std::string& className)
        {
            std::ofstream out(path);

            // Write template C# class
            out << "using System;\nusing System.Collections.Generic;\nusing System.Linq;\nusing System.Text;\nusing System.Threading.Tasks;\nusing FayRuntime;\n";
            out << "namespace FayCore\n{\n";
            out << "    public class " << className << "\n";
            out << "    {\n";
            out << "        public static void OnStart() { } \n";
            out << "        public static void OnUpdate() { } \n";
            out << "    }\n";
            out << "}\n";

            out.close();
        }
}