#include <EngineEditor/EditorCore.h>
namespace Fay
{
	EditorCore::EditorCore() = default;
	EditorCore::~EditorCore() = default;

	void EditorCore::Init()
	{
		m_utils->SetScene(m_context->scene);
		m_utils->SetCamera3D(m_context->camera3D);
		m_utils->SetCamera2D(m_context->camera2D);
		m_utils->SetShader(m_context->shader);
		m_utils->SetRenderLayer(m_context->layer);
		m_utils->SetRenderMode(m_context->renderMode);
		m_utils->SetBatchRenderer(m_context->batchRenderer);
		m_utils->SetTextureManager(m_context->textureManager);
		m_utils->SetSelectedEntity(INVALID_ENTITY);
	}

	void EditorCore::rebuildRuntime() const
	{
		std::string command = "dotnet build \"" + ScriptEngine::GetCoreCsProj() + "\" -c Debug -t:Rebuild";
		int result = std::system(command.c_str());
		
		if (result != 0) FAY_LOG_ERROR("Failed to rebuld FayCore.dll");

		FAY_LOG_INFO("FayCore.dll rebuilt successfully.");
	}
	void EditorCore::handleScriptExecution()
	{
		if (!m_utils->GetIsPlaying())
			return;

		auto& scriptEntities = ComponentManager<ScriptComponent>::Get().getEntities();

		auto& scriptNodes = ComponentManager<ScriptComponent>::Get().getNodes();

		// Node scripts
		for (NodeID n : scriptNodes)
		{
			ScriptComponent* comp = ComponentManager<ScriptComponent>::Get().getNodeComponent(n);

			if (!comp)
				continue;

			for (auto& script : comp->scripts)
			{
				if (!script.hasStarted)
				{
					ScriptEngine::InvokeCoreStatic(script.className, "OnStart");
					script.hasStarted = true;
				}
				ScriptEngine::InvokeCoreStatic(script.className, "OnUpdate");
			}
		}
		// Entity scripts
		for (EntityID e : scriptEntities)
		{
			ScriptComponent* comp =
				ComponentManager<ScriptComponent>::Get().getComponent(e);

			if (!comp)
				continue;

			for (auto& script : comp->scripts)
			{
				if (!script.hasStarted)
				{
					ScriptEngine::InvokeCoreStatic(
						script.className,
						"OnStart"
					);

					script.hasStarted = true;
				}

				ScriptEngine::InvokeCoreStatic(
					script.className,
					"OnUpdate"
				);
			}
		}
	}
	void EditorCore::SetContext(EditorContext& context)
	{
		m_context = &context;
	}
}