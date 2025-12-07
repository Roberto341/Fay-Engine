#include <EngineEditor/EditorUI.h>

namespace Fay
{
	EditorUI::EditorUI() = default;
	EditorUI::~EditorUI() = default;

	void EditorUI::DrawEntitiesPanel()
	{
		// Implementation for drawing the entities panel in the editor UI
#pragma region EntitiesAddDelete
		ImGui::Begin("Entities");
		if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
		{
			if (ImGui::Button("Add Sprite"))
			{
				EntityID entity = m_utils->GetScene()->getNextId();
				auto* sprite = new Sprite(entity, 0, 0, 0, 100, 100, 0, Vec4(1, 0, 0, 1));
				ComponentManager<SpriteComponent>::Get().addComponent(entity, SpriteComponent(sprite));
				m_utils->GetScene()->addObject(sprite);
			}

			if (ImGui::Button("Delete Sprite") && m_utils->GetSelectedEntity() != -1)
			{
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(m_utils->GetSelectedEntity());
				if(comp && comp->sprite)
				{
					m_utils->GetRenderLayer()->remove(comp->sprite);
					m_utils->GetScene()->removeObject(comp->sprite);
					ComponentManager<SpriteComponent>::Get().removeComponent(m_utils->GetSelectedEntity());
					m_utils->SetSelectedEntity(-1);
				}
			}
		}
		else if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
		{
			if(ImGui::Button("Add Cube"))
			{
				EntityID entity = m_utils->GetScene()->getNextId();
				auto* cube = new Cube(entity, 0, 0, 0, 1, 1, 1, Vec4(1, 0, 0, 1));
				ComponentManager<CubeComponent>::Get().addComponent(entity, CubeComponent(cube));
				m_utils->GetScene()->addObject(cube);
			}

			if(ImGui::Button("Delete Cube") && m_utils->GetSelectedEntity() != -1)
			{
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(m_utils->GetSelectedEntity());
				if(comp && comp->cube)
				{
					m_utils->GetRenderLayer()->remove(comp->cube);
					m_utils->GetScene()->removeObject(comp->cube);
					ComponentManager<CubeComponent>::Get().removeComponent(m_utils->GetSelectedEntity());
					m_utils->SetSelectedEntity(-1);
				}
			}
		}
		ImGui::End();

#pragma endregion 
#pragma region EntityComponents
		ImGui::Begin("Entity Components");
		if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
		{
			if (m_utils->GetSelectedEntity() != -1)
			{
				EntityID entity = m_utils->GetSelectedEntity();
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity);

				if (!comp) return;

				auto* sprite = comp->sprite;
				if (ComponentManager<TransformComponent>::Get().hasComponent(entity))
				{
					if (ImGui::Button("Remove Transform"))
					{
						ComponentManager<TransformComponent>::Get().removeComponent(entity);
					}
				}
				else
				{
					if (ImGui::Button("Add Transform"))
					{
						TransformComponent transform(sprite->getPosition(), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(sprite->getSize()));
						ComponentManager<TransformComponent>::Get().addComponent(entity, transform);
					}
				}

				if (ComponentManager<CollisionComponent>::Get().hasComponent(entity))
				{
					if (ImGui::Button("Remove Collision"))
					{
						ComponentManager<CollisionComponent>::Get().removeComponent(entity);
					}
				}
				else
				{
					if (ImGui::Button("Add Collision"))
					{
						CollisionComponent hitBox(sprite->getPosition(), sprite->getSize());
						ComponentManager<CollisionComponent>::Get().addComponent(entity, hitBox);
					}
				}

				if (ComponentManager<ScriptComponent>::Get().hasComponent(entity))
				{
					if (ImGui::Button("Remove Script"))
					{
						ComponentManager<ScriptComponent>::Get().removeComponent(entity);
					}
				}
				else
				{
					if (ImGui::Button("Add Script"))
					{
						ImGui::OpenPopup("EntityScriptPop");
					}
					if (ImGui::BeginPopup("EntityScriptPop"))
					{
						static char scriptName[256] = "Untitled";

						ImGui::InputText("Script Name", scriptName, sizeof(scriptName));

						if (ImGui::Button("Add Existing"))
						{
							std::string className = scriptName;

							if (classExists(className))
							{
								if (m_utils->GetSelectedEntity() == -1) m_utils->SetCurrentSpriteComponent(nullptr);
								ComponentManager<ScriptComponent>::Get().addComponent(entity, ScriptComponent(className, entity));
								auto* comp = ComponentManager<ScriptComponent>::Get().getComponent(entity);
								m_utils->SetCurrentSpriteComponent(ComponentManager<SpriteComponent>::Get().getComponent(entity));
								if (comp)
								{
									if (!comp->hasStarted)
									{
										ScriptEngine::InvokeCoreStatic(comp->className, "OnStart");
										comp->hasStarted = true;
									}
									ScriptEngine::InvokeCoreStatic(comp->className, "OnUpdate");
								}
								else
								{
									FAY_LOG_ERROR("Failed to get ScriptComponent after adding it!");
								}
							}
							else
							{
								FAY_LOG_ERROR("Script class not found in DLL: " << className);
							}
							ImGui::CloseCurrentPopup();
						}
						if (ImGui::Button("Create"))
						{
							std::string fileName = std::string(scriptName) + ".cs";
							std::string fullPath = "Source/Fay/Scripting/FayCore/" + fileName;

							ScriptEngine::createScriptTemplate(fullPath, entity);

							//rebuildRuntime();
							m_core->rebuildRuntime();
							std::string dllPath = "Source/Fay/Scripting/FayCore/bin/Debug/net48/FayCore.dll";

							// Reload in Mono[
							ScriptEngine::ReloadAssembly(dllPath);
							ScriptEngine::InvokeCoreStatic(scriptName, "OnStart");
							ImGui::CloseCurrentPopup();
						}
						if (ImGui::Button("Close"))
						{
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}
			}
		}
		else if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
		{
			if (m_utils->GetSelectedEntity() != -1)
			{
				EntityID entity = m_utils->GetSelectedEntity();
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(entity);

				if (!comp)
					return;
				auto* cube = comp->cube;

				if (ComponentManager<TransformComponent>::Get().hasComponent(entity))
				{
					if (ImGui::Button("Remove Transform"))
					{
						ComponentManager<TransformComponent>::Get().removeComponent(entity);
					}
				}
				else
				{
					if (ImGui::Button("Add Transform"))
					{
						TransformComponent transform(cube->getPosition(), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(cube->getSize()));
						ComponentManager<TransformComponent>::Get().addComponent(entity, transform);
					}
				}

				if (ComponentManager<CollisionComponent>::Get().hasComponent(entity))
				{
					if (ImGui::Button("Remove Collision"))
					{
						ComponentManager<CollisionComponent>::Get().removeComponent(entity);
					}
				}
				else
				{
					if (ImGui::Button("Add Collision"))
					{
						CollisionComponent hitBox(cube->getPosition(), cube->getSize());
						ComponentManager<CollisionComponent>::Get().addComponent(entity, hitBox);
					}
				}
			}
		}
		ImGui::End();
#pragma endregion
#pragma region EntityProperties
		ImGui::Begin("Entity Properties");

		if (m_utils->GetSelectedEntity() != -1)
		{
			if (auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(m_utils->GetSelectedEntity()))
			{
				m_utils->drawEntityColorUI(m_utils->GetSelectedEntity(), comp->sprite, comp);
			}
			else if (auto* comp = ComponentManager<CubeComponent>::Get().getComponent(m_utils->GetSelectedEntity()))
			{
				m_utils->drawEntityColorUI(m_utils->GetSelectedEntity(), comp->cube, comp);
			}
		}
		ImGui::End();
#pragma endregion
	}
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
				m_utils->SetPendingMode((RenderMode)currentMode);
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
				std::string filename = std::string(sceneName) + ".fayScene";
				std::string fullPath = "Res/Assets/Scenes/" + filename;
				m_utils->CreateScene(fullPath);
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