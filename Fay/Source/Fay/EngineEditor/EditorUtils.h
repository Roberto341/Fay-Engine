#pragma once
#include <ImGui/imgui.h>
#include <Graphics/Camera3D.h>
#include <Graphics/Shader.h>
#include <Graphics/Layers/TileLayer.h>

#include <Renderer/Scene.h>
#include <Renderer/Renderable.h> // ensure Renderable is visible
#include <Entity/Components.h>
namespace Fay
{
	enum class RenderMode
	{
		MODE_2D,
		MODE_3D
	};
	class EditorUtils
	{
	public:
		EditorUtils();
		~EditorUtils();

		// --- Static ---
		// --- Entity ---
		static EntityID GetSelectedEntity() { return m_selectedEntity; }
		static EntityID m_selectedEntity;
		static float s_entitySpeed;
		static SpriteComponent* s_currentSpriteComponent;
		// --- Scene Management ---
		static Scene* s_Scene;
		void SetStaticScene() { s_Scene = m_scene; }
		static size_t GetSceneObjects() { return s_Scene->getObjects().size(); }

		static SceneType s_ActiveScene;
		static SceneType GetCurrentScene() { return s_ActiveScene; }
		static void SetActiveScene(SceneType type) { s_ActiveScene = type; }
		static bool IsSceneActive() { return s_ActiveScene == SceneType::Scene2D || s_ActiveScene == SceneType::Scene3D; }

		// --- Getters ---
		Camera3D* GetCamera() const { return m_camera; }
		Shader* GetShader() const { return m_shader; }
		Scene* GetScene() const { return m_scene; }
		TileLayer* GetRenderLayer() const { return m_renderLayer; }
		RenderMode GetRenderMode() { return m_renderMode; }
		RenderMode GetPendingMode() { return m_pendingMode; }
		bool GetModeUpdate() { return m_pendingModeUpdate; }
		bool GetSkipNextFrame() { return m_skipNextFrame; }
		bool GetNewSceneReq() { return m_newSceneRequested; }
		bool GetLoadSceneReq() { return m_loadSceneRequested; }
		static float GetEntitySpeed() { return s_entitySpeed; }
		// --- Setters ---
		void SetCamera(Camera3D* camera) { m_camera = camera; }
		void SetShader(Shader* shader) { m_shader = shader; }
		void SetScene(Scene* scene) { m_scene = scene; }
		void SetRenderLayer(TileLayer* layer) { m_renderLayer = layer; }
		void SetRenderMode(RenderMode mode) { m_renderMode = mode; }
		void SetPendingMode(RenderMode penMode) { m_pendingMode = penMode; }
		void SetSelectedEntity(EntityID id) { m_selectedEntity = id; }
		void SetTextureManager(const TextureManager& texMan) { m_texManager = texMan; }
		void SetBatchRenderer(BatchRenderer* batch) { m_batchRenderer = batch; }
		void SetCurrentSpriteComponent(SpriteComponent* comp) { s_currentSpriteComponent = comp; }
		void SetModeUpdate(bool yn) { m_pendingModeUpdate = yn; }
		void SetSkipNextFrame(bool yn) { m_skipNextFrame = yn; }
		void SetNewSceneReq(bool yn) { m_newSceneRequested = yn; }
		void SetLoadSceneReq(bool yn) { m_loadSceneRequested = yn; }
		void SetEntitySpeed(float speed) { s_entitySpeed = speed; }

		void SaveScene();
		void CreateScene(const std::string& path);
		void LoadScene(const std::string& path);
		void DeleteScene();
		void applyPendingMode();

		template<typename TComponent, typename TObject>
		void drawEntityColorUI(EntityID entity, TObject* object, TComponent* comp)
		{
			if (!object || !comp)
				return;

			char idBuf[32];
			sprintf(idBuf, "%d", entity);
			ImGui::InputText("Entity ID", idBuf, IM_ARRAYSIZE(idBuf), ImGuiInputTextFlags_ReadOnly);

			Vec4 col = object->getColor();
			float color[4] = { col.x, col.y, col.z, col.w };

			if (ImGui::ColorEdit4("Color", color))
			{
				object->setColor(Vec4(color[0], color[1], color[2], color[3]));
				comp->setColor(object->getColor());
			}
		}
	private:
		// All other classes
		std::string m_currentScene;
		Scene* m_scene = nullptr;
		Camera3D* m_camera = nullptr;
		Shader* m_shader = nullptr;
		TileLayer* m_renderLayer = nullptr;
		TextureManager m_texManager;
		BatchRenderer* m_batchRenderer = nullptr;
		// RenderMode
		RenderMode m_renderMode = RenderMode::MODE_2D;
		RenderMode m_pendingMode = RenderMode::MODE_2D;
		// Scene
		SceneType m_activeScene = SceneType::None;
		bool m_pendingModeUpdate = false;
		bool m_skipNextFrame = false;
		bool m_newSceneRequested = false;
		bool m_loadSceneRequested = false;
	};
}