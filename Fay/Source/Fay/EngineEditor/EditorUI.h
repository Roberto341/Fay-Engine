#pragma once
#include <ImGui/imgui_internal.h>
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUtils.h>
#include <EngineEditor/Utils/ViewportMode.h>
#include <Scripting/ScriptGlue.h>
#include <unordered_set>
namespace Fay
{
	class EditorViewport;
	class EditorUI
	{

	public:
		EditorUI();
		~EditorUI();
		// Draw
		void DrawEntitiesPanel();
		void DrawFolder(const std::filesystem::path& path);
		void DrawFile(const std::filesystem::path& path);
		void DrawFileMenu();
		void DrawDockspace();
		void DrawToolsPanel();
		void DrawContentBrowser();
		void DrawCodeEditor();
		void SetupCodeEditor(const char* label, std::string& text_buffer);

		// Setters
		void SetCore(EditorCore* core) { m_core = core; }
		void SetUtils(EditorUtils* util) { m_utils = util; }
		void SetViewport(EditorViewport* viewport) { m_viewport = viewport; }
		void SetCurrentScript(const std::string& script) { m_currentScript = script; }
		
		// Reset
		void ResetScriptState();
	private:
		EditorUtils* m_utils = nullptr;
		EditorCore* m_core = nullptr;
		EditorViewport* m_viewport = nullptr;
		std::filesystem::path m_currentDir = "Res/Assets";
		std::string m_currentScript = "";
	};
}