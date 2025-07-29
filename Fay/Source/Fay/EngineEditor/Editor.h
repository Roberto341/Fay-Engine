#pragma once
#include <Renderer/Scene2D.h>
#include <Renderer/Scene3D.h>
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
#define TEST 0
namespace Fay
{
	enum class RenderMode
	{
		MODE_2D,
		MODE_3D
	};
	class Editor
	{
	public:
		Editor(Window& window);
		void loadScene(const std::string& path);
		void saveScene(const std::string& path);
		~Editor();

		void run();
	private:
		// Camera stuff
		void cameraUpdate();
		// Shader stuff
		Shader* m_shader;
		Shader* m_shader3d;
		void setShader(Fay::Shader* shader);
		void setShader3D(Fay::Shader* shader);
		RenderMode m_renderMode = RenderMode::MODE_2D; // default to 2d rendering
		// File
		bool showSaveDialog = false;
		bool showLoadDialog = false;
		void setupDockspace();
		void setupFileMenu();
		int selectedSpriteIndex;
		int selectedCubeIndex;
		float m_lastTime;
		// Misc
		Camera3D* m_camera3D;
		TileLayer* m_renderLayer;
		TileLayer* m_renderLayer3D;
		Scene2D m_scene2D;
		Scene3D m_scene3D;
		TextureManager m_textureManager;
		Window& m_window;
		FrameBuffer m_framebuffer;
	};
}