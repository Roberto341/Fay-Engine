#pragma once
#include <ImGui/imgui.h>
#include <Graphics/Camera3D.h>
#include <Graphics/Camera2D.h>
#include <Graphics/Shader.h>
#include <Graphics/Layers/TileLayer.h>

#include <Renderer/Scene.h>
#include <Renderer/Renderable.h>
#include <Entity/Components.h>
namespace Fay
{
	class EntityFactory;
	class EditorViewport;
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

		// --- Scene Management ---
		static Scene* s_Scene;
		void SetStaticScene() { s_Scene = m_scene; }
		static size_t GetSceneObjects() { return s_Scene->getObjects().size(); }

		static SceneType s_ActiveScene;
		static SceneType GetCurrentScene() { return s_ActiveScene; }
		static void SetActiveScene(SceneType type) { s_ActiveScene = type; }
		static bool IsSceneActive() { return s_ActiveScene == SceneType::Scene2D || s_ActiveScene == SceneType::Scene3D; }

		// --- Getters ---
		Camera3D* GetCamera3D() const { return m_camera3D; }
		Camera2D* GetCamera2D() const { return m_camera2D; }

		Shader* GetShader() const { return m_shader; }
		Scene* GetScene() const { return m_scene; }
		TileLayer* GetRenderLayer() const { return m_renderLayer; }
		RenderMode GetRenderMode() { return m_renderMode; }
		RenderMode GetPendingMode() { return m_pendingMode; }

		bool GetModeUpdate() { return m_pendingModeUpdate; }
		bool GetSkipNextFrame() { return m_skipNextFrame; }
		bool GetNewSceneReq() { return m_newSceneRequested; }
		bool GetLoadSceneReq() { return m_loadSceneRequested; }
		bool classExists(const std::string& className) const
		{
			return ScriptEngine::GetMonoClass(className) != nullptr;
		}
		static float GetEntitySpeed() { return s_entitySpeed; }
		// --- Setters ---
		void SetCamera3D(Camera3D* camera) { m_camera3D = camera; }
		void SetCamera2D(Camera2D* camera) { m_camera2D = camera; }

		void SetShader(Shader* shader) { m_shader = shader; }
		void SetScene(Scene* scene) { m_scene = scene; EditorUtils::s_Scene = scene; }
		void SetRenderLayer(TileLayer* layer) { m_renderLayer = layer; }
		void SetRenderMode(RenderMode mode) { m_renderMode = mode; }
		void SetPendingMode(RenderMode penMode) { m_pendingMode = penMode; }
		void SetSelectedEntity(EntityID id) { m_selectedEntity = id; }
		void SetTextureManager(const TextureManager& texMan) { m_texManager = texMan; }
		void SetBatchRenderer(BatchRenderer* batch) { m_batchRenderer = batch; }
		void SetModeUpdate(bool yn) { m_pendingModeUpdate = yn; }
		void SetSkipNextFrame(bool yn) { m_skipNextFrame = yn; }
		void SetNewSceneReq(bool yn) { m_newSceneRequested = yn; }
		void SetLoadSceneReq(bool yn) { m_loadSceneRequested = yn; }
		void SetEntitySpeed(float speed) { s_entitySpeed = speed; }

		void SaveScene();
		void CreateScene(const std::string& path);
		void LoadScene(const std::string& path);
		void DeleteScene();
		void applyPendingMode(EditorViewport* viewport);

		// EntityFactory
		EntityFactory* GetFactory() const { return m_factory; }
		void SetFactory(EntityFactory* factory) { m_factory = factory; }
		
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

			// if entity has a texture ignore this
			if (ImGui::ColorEdit4("Color", color))
			{
				object->setColor(Vec4(color[0], color[1], color[2], color[3]));
				comp->setColor(object->getColor());
			}
			// if entity has a texture, show here
		}
		void drawEntityScriptList(EntityID entity)
		{
			if (entity == INVALID_ENTITY)
				return;

			auto* scriptComp = ComponentManager<ScriptComponent>::Get().getComponent(entity);
			// If no ScriptComponent, allow adding one
			if (!scriptComp)
			{
				ImGui::TextDisabled("ScriptComponent invalid.");
				return;
			}

			static EntityID lastEntity = INVALID_ENTITY;
			static int selectedIndex = -1;

			if (lastEntity != entity)
			{
				selectedIndex = -1;
				lastEntity = entity;
			}

			// List attached scripts
			if (scriptComp->scripts.empty())
			{
				ImGui::TextDisabled("No scripts attached.");
			}
			else
			{
				if (ImGui::BeginListBox("Scripts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (int i = 0; i < (int)scriptComp->scripts.size(); i++)
					{
						bool selected = (selectedIndex == i);
						if (ImGui::Selectable(scriptComp->scripts[i].className.c_str(), selected))
							selectedIndex = i;
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}

				// Remove selected script
				if (selectedIndex >= 0 && selectedIndex < (int)scriptComp->scripts.size())
				{
					if (ImGui::Button("Remove Script"))
					{
						scriptComp->removeScript(selectedIndex);
						selectedIndex = -1;
					}
				}
			}

			// Add Existing / Add New Script buttons
			if (ImGui::Button("Add Script"))
			{
				ImGui::OpenPopup("AddScriptPopup");
			}

			if (ImGui::BeginPopup("AddScriptPopup"))
			{
				static char scriptName[256] = "Untitled";
				ImGui::InputText("Script Name", scriptName, sizeof(scriptName));

				if (ImGui::Button("Add Existing"))
				{
					std::string className = scriptName;

					if (classExists(className))
					{
						if (!scriptComp)
						{
							ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(entity));
							scriptComp = ComponentManager<ScriptComponent>::Get().getComponent(entity);
						}

						// Prevent duplicate scripts
						bool exists = false;
						for (auto& s : scriptComp->scripts)
							if (s.className == className) exists = true;

						if (!exists)
						{
							scriptComp->scripts.emplace_back(className);
							auto& newScript = scriptComp->scripts.back();
							if (!newScript.hasStarted)
							{
								ScriptEngine::InvokeCoreStatic(newScript.className, "OnStart");
								newScript.hasStarted = true;
							}
						}
						else
						{
							FAY_LOG_WARN("Script already attached to entity: " << className);
						}
					}
					else
					{
						FAY_LOG_ERROR("Script class not found: " << className);
					}

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Create New"))
				{
					std::string path = "Source/Fay/Scripting/FayCore/" + std::string(scriptName) + ".cs";
					ScriptEngine::createScriptTemplate(path, entity);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}
	private:
		// All other classes
		std::string m_currentScene;
		Scene* m_scene = nullptr;
		Camera3D* m_camera3D = nullptr;
		Camera2D* m_camera2D = nullptr;
		Shader* m_shader = nullptr;
		TileLayer* m_renderLayer = nullptr;
		TextureManager m_texManager;
		BatchRenderer* m_batchRenderer = nullptr;
		// RenderMode
		RenderMode m_renderMode = RenderMode::MODE_2D;
		RenderMode m_pendingMode = RenderMode::MODE_2D;
		EntityFactory* m_factory = nullptr;
		// Scene
		SceneType m_activeScene = SceneType::Scene2D;
		bool m_pendingModeUpdate = false;
		bool m_skipNextFrame = false;
		bool m_newSceneRequested = false;
		bool m_loadSceneRequested = false;
	};
}