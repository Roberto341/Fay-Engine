#pragma once
#include <Graphics/Window.h>
#include <Graphics/Buffers/FrameBuffer.h>
#include <Graphics/Layers/TileLayer.h>
#include <Graphics/Camera2D.h>
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
#include <Math/Math.h>
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUI.h>
#include <EngineEditor/EditorViewport.h>
#include <EngineEditor/EditorUtils.h>
#include <Entity/EntityFactory.h>
namespace Fay
{
	class EditorViewport;
	class EditorCore;
	class EditorUI;
	class EditorUtils;
	class Editor
	{
	public:
		Editor();
		~Editor();

		void runEditor();
		void setupEditor();
		void initImgui();
		Window& getWindow() { return *m_window; }
		EditorUtils& getUtils() { return *m_utils; }
	private:
		std::unique_ptr<EditorViewport> m_viewport;
		std::unique_ptr<EditorCore> m_core;
		std::unique_ptr<EditorUI> m_ui;
		std::unique_ptr<EditorUtils> m_utils;
		std::unique_ptr<Window> m_window;
		std::unique_ptr<EntityFactory> m_factory;
		// Shader
		Shader* m_shader;
		// RenderMode
		RenderMode m_renderMode = RenderMode::MODE_2D;
		// Misc
		Camera3D* m_camera3D;
		TileLayer* m_renderLayer;
		Scene m_Scene;
		TextureManager m_textureManager;
		FrameBuffer m_framebuffer;
		BatchRenderer* m_batchRenderer;
		// Scene Management
		SceneType m_activeScene = SceneType::None;
	};
}