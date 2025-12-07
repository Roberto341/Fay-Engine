#pragma once
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUtils.h>

namespace Fay
{
	class EditorUI
	{

	public:
		EditorUI();
		~EditorUI();
		void DrawEntitiesPanel();
		void DrawFileMenu();
		void DrawDockspace();
		void DrawToolsPanel();
		bool classExists(const std::string& className) const
		{
			return ScriptEngine::GetMonoClass(className) != nullptr;
		}
		void SetCore(EditorCore* core) { m_core = core; }
		void SetUtils(EditorUtils* util) { m_utils = util; }
	private:
		EditorUtils* m_utils = nullptr;
		EditorCore* m_core = nullptr;

	};
}