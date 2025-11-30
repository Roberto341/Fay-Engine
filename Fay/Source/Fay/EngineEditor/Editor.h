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
#include <EngineEditor/Configuration.h>
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
		// Entity
		void SetSelectedEntity(EntityID id);
		static EntityID GetSelEntity();
		static EntityID s_SelectedEntity;
		static float s_EntitySpeed;
		void SetEntitySpeed(float speed);
		static float GetEntitySpeed();
		// Scene mangagement
		static SceneType s_ActiveScene;
		static Scene* s_Scene;
		void SetScene();
		static size_t GetSceneObjects();
		static bool IsSceneActive();
		static SceneType GetCurrentScene();
		static void SetActiveScene(SceneType type);
		
		// Scene and Configuration list's
		static bool shouldRefreshScenes;
		static bool shouldRefreshConfigs;

		// For scene creation
		static bool newSceneRequested; 
	private:
		// Camera stuff
		void cameraUpdate();
		// Shader stuff
		Shader* m_shader;
		RenderMode m_renderMode = RenderMode::MODE_2D;
		// File
		// Scene
		void saveCurrentScene();
		void createScene(const std::string& path);
		// ImGui stuff
		void setupDockspace();
		void setupTools();
		void setupFileMenu();
		void setupViewport();
		void setupRenderMode();
		void setupEntityPanel() const;
		void setupWireFrame() const;
		void setSceneBox();
		void setConfigBox();
		EntityID selectedEntityID;
		float m_lastTime;
		// Current path
		std::string m_currentScenePath;
		std::string m_currentConfigPath;
		// Misc
		Camera3D* m_camera3D;
		TileLayer* m_renderLayer;
		Scene m_Scene;
		Configuration m_Configuration;
		TextureManager m_textureManager;
		Window& m_window;
		FrameBuffer m_framebuffer;
		BatchRenderer* m_batchRenderer;
		// Scene Management
		SceneType m_activeScene = SceneType::None;
		// Ray tracing
		bool intersectRayAABB(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& aabbMin, const Vec3& aabbMax, float& t);
		Ray getRayFromMouse(const ImVec2& mousePos, const ImVec2& viewportPos, const ImVec2& viewportSize, const Mat4& proj, const Mat4& view);
		// Tile map
		void loadPalette(const std::string& path);
		void savePalette(const std::string& path);
		void createPalette(const std::string& path);
		void saveCurrentPalette();
	};
}