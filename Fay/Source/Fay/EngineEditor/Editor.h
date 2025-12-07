#pragma once
#include <Graphics/Window.h>
#include <Graphics/Buffers/FrameBuffer.h>
#include <Graphics/Layers/TileLayer.h>
#include <Graphics/Camera.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileDialog.h>
#include <ImGuizmo/ImGuizmo.h>
#include <Graphics/Camera3D.h>
#include <Entity/ComponentManager.h>
#include <Entity/Components.h>
#include <Scripting/ScriptEngine.h>
#include <Scripting/ScriptGlue.h>
#include <Renderer/Scene.h>
#include <EngineEditor/Configuration.h>
#include <Math/Math.h>
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUI.h>
#include <EngineEditor/EditorViewport.h>
namespace Fay
{
	class Editor
	{
	private:
	public:
	public:
		Editor(Window& window);
		~Editor();

		void run();
	private:
		std::unique_ptr<EditorViewport> m_viewport;
		std::unique_ptr<EditorCore> m_core;
		std::unique_ptr<EditorUI> m_ui;
		std::unique_ptr<EditorUtils> m_utils;
		// Shader
		Shader* m_shader;
		// RenderMode
		RenderMode m_renderMode = RenderMode::MODE_2D;
		// Current path
		// Misc
		Camera3D* m_camera3D;
		TileLayer* m_renderLayer;
		Scene m_Scene;
		TextureManager m_textureManager;
		Window& m_window;
		FrameBuffer m_framebuffer;
		BatchRenderer* m_batchRenderer;
		// Scene Management
		SceneType m_activeScene = SceneType::None;
	};
}