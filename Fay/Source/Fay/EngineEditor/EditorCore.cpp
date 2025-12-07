#include <EngineEditor/EditorCore.h>

namespace Fay
{
	EditorCore::EditorCore() = default;
	EditorCore::~EditorCore() = default;

	void EditorCore::Init(Scene* scene, Camera3D* camera, Shader* shader, TileLayer* layer, RenderMode mode, BatchRenderer* batch, TextureManager texMan)
	{
		m_utils->SetScene(scene);
		m_utils->SetCamera(camera);
		m_utils->SetShader(shader);
		m_utils->SetRenderLayer(layer);
		m_utils->SetRenderMode(mode);
		m_utils->SetBatchRenderer(batch);
		m_utils->SetTextureManager(texMan);
		m_utils->SetSelectedEntity(-1);
	}

	void EditorCore::rebuildRuntime() const
	{
		std::string command = "dotnet build Source/Fay/Scripting/FayCore/FayCore.csproj /p:Configuration=Debug /t:Rebuild";
		
		int result = std::system(command.c_str());
		
		if (result != 0) FAY_LOG_ERROR("Failed to rebuld FayCore.dll");

		FAY_LOG_INFO("FayCore.dll rebuilt successfully.");
	}
	void EditorCore::handleScriptExecution()
	{
		static bool assemblyLoaded = false;
		if (m_utils->GetSelectedEntity() == -1) m_utils->SetCurrentSpriteComponent(nullptr);
		auto* comp = ComponentManager<ScriptComponent>::Get().getComponent(m_utils->GetSelectedEntity());
		m_utils->SetCurrentSpriteComponent(ComponentManager<SpriteComponent>::Get().getComponent(m_utils->GetSelectedEntity()));
		if (comp)
		{
			if (!assemblyLoaded)
			{
				std::string dllPath = "Source/Fay/Scripting/FayCore/bin/Debug/net48/FayCore.dll";
				ScriptEngine::ReloadAssembly(dllPath);
				assemblyLoaded = true;
			}

			if (!comp->hasStarted)
			{
				ScriptEngine::InvokeCoreStatic(comp->className, "OnStart");
				comp->hasStarted = true;
			}
			ScriptEngine::InvokeCoreStatic(comp->className, "OnUpdate");
		}
	}
}