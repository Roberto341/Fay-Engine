#include <EngineEditor/EditorUI.h>

namespace Fay
{
	EditorUI::EditorUI() = default;
	EditorUI::~EditorUI() = default;

	void EditorUI::DrawEntitiesPanel()
	{
		// Implementation for drawing the entities panel in the editor UI
#pragma region Hierarchy
		ImGui::Begin("Hierarchy");

		auto* scene = m_utils->GetScene();
		
		ImGui::Text("Scene %s", "Untitled"); // Replace later
		ImGui::Separator();

		// Add/Delete buttons inline
		if(ImGui::Button("+")) 
		{
			EntityID id = m_utils->GetFactory()->CreateEntity(m_utils->GetRenderMode());
			//FAY_LOG_DEBUG("Entity created with ID: " << id);
			m_utils->SetSelectedEntity(id);
		}
		ImGui::SameLine();
		ImGui::Text("Hierarchy panel alive");
		if (ImGui::Button("-")) 
		{
			EntityID id = m_utils->GetSelectedEntity();
			if (id != INVALID_ENTITY)
			{
				FAY_LOG_DEBUG("Destroying entity");
				m_utils->GetScene()->destroyEntity(id);
				//m_utils->SetSelectedEntity(INVALID_ENTITY);
			}

			/* delete 2d or 3d */ 
		}

		ImGui::Separator();

		if (scene->getObjectCount() > 0)
		{
			if(ImGui::BeginListBox("##Hierarchy objects", ImVec2(-FLT_MIN, -FLT_MIN)))
			{
				for (auto* obj : scene->getObjects())
				{
					std::string name = "Object: " + std::to_string(obj->getId());

					if (ImGui::Selectable(name.c_str(),
						m_utils->GetSelectedEntity() == obj->getId()))
					{
						m_utils->SetSelectedEntity(obj->getId());
					}
				}
				ImGui::EndListBox();
			}
		}
		else
		{
			ImGui::TextDisabled("Empty Scene.");
		}

		ImGui::End();

#pragma endregion 
#pragma region EntityComponents
		ImGui::Begin("Inspector");

		EntityID entity = m_utils->GetSelectedEntity();

		if (entity != INVALID_ENTITY)
		{
			ImGui::Text("Entity: %d", entity);
			ImGui::Separator();

			// -----------------------
			// Components Section
			// -----------------------

			if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
				{
					auto* comp = ComponentManager<CubeComponent>::Get().getComponent(entity);

					if (comp)
					{
						auto* cube = comp->cube;

						// Transform
						if (ComponentManager<TransformComponent>::Get().hasComponent(entity))
						{
							if(ImGui::Button("Remove Transform"))
								ComponentManager<TransformComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Transform"))
						{
							TransformComponent transform(
								cube->getPosition(),
								Vec3(0, 0, 0),
								Vec3(0, 0, 0),
								Vec3(cube->getSize())
							);
							ComponentManager<TransformComponent>::Get().addComponent(entity, transform);
						}
						if (ComponentManager<CollisionComponent>::Get().hasComponent(entity))
						{
							if (ImGui::Button("Remove Collision"))
								ComponentManager<CollisionComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Collision"))
						{
							CollisionComponent hitBox(
								cube->getPosition(),
								cube->getSize()
							);
							ComponentManager<CollisionComponent>::Get().addComponent(entity, hitBox);
						}
						// Script 
						if (ComponentManager<ScriptComponent>::Get().hasComponent(entity))
						{
							if (ImGui::Button("Remove Script Component"))
								ComponentManager<ScriptComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Script Component"))
						{
							ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(entity));
						}
					}
				}
				if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
				{
					auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity);

					if (comp)
					{
						auto* sprite = comp->sprite;

						// Transform
						if (ComponentManager<TransformComponent>::Get().hasComponent(entity))
						{
							if (ImGui::Button("Remove Transform"))
								ComponentManager<TransformComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Transform"))
						{
							TransformComponent transform(
								sprite->getPosition(),
								Vec3(0, 0, 0),
								Vec3(0, 0, 0),
								Vec3(sprite->getSize())
							);
							ComponentManager<TransformComponent>::Get().addComponent(entity, transform);
						}
						// Collision
						if (ComponentManager<CollisionComponent>::Get().hasComponent(entity))
						{
							if (ImGui::Button("Remove Collision"))
								ComponentManager<CollisionComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Collision"))
						{
							CollisionComponent hitBox(
								sprite->getPosition(),
								sprite->getSize()
							);
							ComponentManager<CollisionComponent>::Get().addComponent(entity, hitBox);
						}
						// Script 
						if (ComponentManager<ScriptComponent>::Get().hasComponent(entity))
						{
							if (ImGui::Button("Remove Script Component"))
								ComponentManager<ScriptComponent>::Get().removeComponent(entity);
						}
						else if (ImGui::Button("Add Script Component"))
						{
							ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(entity));
						}
					}
				}
			}
			ImGui::Separator();

			// -----------------------
			// Properties Section
			// -----------------------
			if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity))
				{
					m_utils->drawEntityColorUI(entity, comp->sprite, comp);
				}
				else if (auto* comp = ComponentManager<CubeComponent>::Get().getComponent(entity))
				{
					m_utils->drawEntityColorUI(entity, comp->cube, comp);
				}
				m_utils->drawEntityScriptList(entity);
			}
		}
		ImGui::End();
}
#pragma endregion
#pragma region DrawFileMenu
	void EditorUI::DrawFileMenu()
	{
		ImGui::Begin("File");
		
		const char* modes[] = { "2D", "3D" };
		static int currentMode = 0;
		
		currentMode = (int)m_utils->GetRenderMode();
		
		if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
		{
			if (m_utils->GetScene()->canSwitchScene())
			{
				RenderMode newMode = (RenderMode)currentMode;
				m_utils->SetPendingMode(newMode);
				m_utils->SetModeUpdate(true);
			}
		}
		ImGui::End();
	}
#pragma endregion
#pragma region DrawDockspace
	void EditorUI::DrawDockspace()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		// DockSpace window style
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_MenuBar;

		ImGui::Begin("DockSpace", nullptr, flags);
		ImGui::PopStyleVar(2);
		
		// -------------------------------
		// Main menu bar
		// -------------------------------
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					m_utils->SetNewSceneReq(true);
				}
				if (ImGui::MenuItem("Load Scene"))
				{
					m_utils->SetLoadSceneReq(true);
				}
				if(ImGui::MenuItem("Save Scene"))
				{
					m_utils->SaveScene();
				}
				if (ImGui::MenuItem("Delete Scene"))
				{
					m_utils->DeleteScene();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
		ImGui::EndMenuBar();
		}
		// -------------------------------
		// Dockspace region
		// -------------------------------

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();

		// Outside menu logic:
		if (m_utils->GetLoadSceneReq())
		{
			ImGui::OpenPopup("LoadScenePop");
			m_utils->SetLoadSceneReq(false);
		}
		if (m_utils->GetNewSceneReq())
		{
			ImGui::OpenPopup("NewScenePop");
			m_utils->SetNewSceneReq(false);
		}

		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
		if (ImGui::BeginPopup("NewScenePop"))
		{
			static char sceneName[256] = "UntitledScene";
			
			ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));

			if (ImGui::Button("Create"))
			{
				MonoDomain* domain = mono_domain_get();
				MonoString* monoSceneName = mono_string_new(domain, sceneName);
				ScriptGlue::InternalCalls_Scene_CreateScene(monoSceneName);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("LoadScenePop", nullptr))
		{
			static const std::string sceneDir = "Res/Assets/Scenes/";
			static std::vector<std::string> scenes;
			static int selectedSceneIndex = -1;

			if (ImGui::IsWindowAppearing())
				scenes = m_utils->GetScene()->listScenesDir(sceneDir);

			ImGui::Text("Select a Scene: ");
			ImGui::Separator();

			for (const auto& file : scenes)
			{
				if (ImGui::Selectable(file.c_str()))
				{
					m_utils->LoadScene(sceneDir + file);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();
	}
	void EditorUI::DrawToolsPanel()
	{
		ImGui::Begin("Tools");

		if (ImGui::SliderFloat("Entity Speed", &EditorUtils::s_entitySpeed, 0, 10))
		{
			m_utils->SetEntitySpeed(m_utils->GetEntitySpeed());
		}
		ImGui::End();
	}
#pragma endregion
}