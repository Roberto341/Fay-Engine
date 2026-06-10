#pragma once
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUtils.h>
#include <EngineEditor/Utils/ViewportMode.h>
#include <Scripting/ScriptGlue.h>
namespace Fay
{
	class EditorViewport;
	class EditorUI
	{

	public:
		EditorUI();
		~EditorUI();
		void DrawEntitiesPanel();
		void DrawFileMenu();
		void DrawDockspace();
		void DrawToolsPanel();
		void SetCore(EditorCore* core) { m_core = core; }
		void SetUtils(EditorUtils* util) { m_utils = util; }
		void SetViewport(EditorViewport* viewport) { m_viewport = viewport; }
	private:
		EditorUtils* m_utils = nullptr;
		EditorCore* m_core = nullptr;
		EditorViewport* m_viewport = nullptr;
	};
}