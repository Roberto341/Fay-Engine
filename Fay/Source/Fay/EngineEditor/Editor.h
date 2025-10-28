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
#include <Scripting/ScriptSystem.h>
#include <Renderer/Scene.h>
#define TEST 0
namespace Fay
{
	enum class RenderMode
	{
		MODE_2D,
		MODE_3D
	};
	struct Ray
	{
		Vec3 origin;
		Vec3 dir;
	};
	class Editor
	{
	public:
		Editor(Window& window);
		void loadScene(const std::string& path);
		void saveScene(const std::string& path);
		~Editor();

		void run();
		void SetSelectedEntity(EntityID id);
		static EntityID GetSelEntity();
		static EntityID s_SelectedEntity;
		static SceneType s_ActiveScene;
		static bool IsSceneActive();
		static SceneType GetCurrentScene();
		static void SetActiveScene(SceneType type);
		static bool shouldRefreshScenes;
	private:
		// Camera stuff
		void cameraUpdate();
		// Shader stuff
		Shader* m_shader;
		Shader* m_shader3d;
		void setShader(Fay::Shader* shader);
		void setShader3D(Fay::Shader* shader);
		RenderMode m_renderMode = RenderMode::MODE_2D;
		// File
		void saveCurrentScene();
		void createNewScene(const std::string& path);
		bool showSaveDialog = false;
		bool showLoadDialog = false;
		void setupDockspace();
		void setupFileMenu();
		EntityID selectedEntityID;
		float m_lastTime;
		std::string m_currentScenePath;
		// Misc
		Camera3D* m_camera3D;
		TileLayer* m_renderLayer;
		//TileLayer* m_renderLayer3D;
		Scene m_Scene;
		TextureManager m_textureManager;
		Window& m_window;
		FrameBuffer m_framebuffer;
		BatchRenderer* m_batchRenderer;
		// Scene Management
		SceneType m_activeScene = SceneType::None;
		// private methods
		bool intersectRayAABB(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& aabbMin, const Vec3& aabbMax, float& t);
		Ray getRayFromMouse(const ImVec2& mousePos, const ImVec2& viewportPos, const ImVec2& viewportSize, const Mat4& proj, const Mat4& view);
	};
}