#include <EngineEditor/EditorUI.h>

namespace Fay
{
	EditorUI::EditorUI() = default;
	EditorUI::~EditorUI() = default;

#pragma region DrawEntitiesPanel
	void EditorUI::DrawEntitiesPanel()
	{
		// Implementation for drawing the entities panel in the editor UI
		ImGui::Begin("Hierarchy");

		auto* scene = m_utils->GetScene();

		ImGui::TextDisabled("Scene");
		ImGui::SameLine();
		ImGui::Text("%s", m_utils->GetCurrentSceneName().c_str());
		ImGui::Separator();

		// Add/Delete buttons inline
		if (ImGui::Button("Create Node"))
		{
			m_utils->SetShowControlNameBox(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Node"))
		{
			EntityID id = m_utils->GetSelectedNode();
			if (id != INVALID_ENTITY)
			{
				FAY_LOG_DEBUG("Destroying node");
				m_utils->GetScene()->destroyNode(id);
				m_utils->SetSelectedNode(INVALID_NODE);
			}
		}
		ImGui::Separator();
		if (ImGui::Button("Create Entity"))
		{
			EntityID id = m_utils->GetFactory()->CreateEntity(m_utils->GetRenderMode());
			m_utils->SetSelectedEntity(id);

			// Add to ControlNode if a node is selected
			if (m_utils->GetSelectedNode() != INVALID_NODE)
			{
				NodeID nodeId = m_utils->GetSelectedNode();
				ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeId)->addEntity(id);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Entity"))
		{
			EntityID id = m_utils->GetSelectedEntity();

			// before deleting the entity find if it has a parent node and remove there first
			for (int i = 0; i < scene->getNodeCount(); i++)
			{
				//Grab all the nodes in the scene and itarate throught them
				ControlNode* node = scene->getNodeByIndex(i);
				NodeID nodeId = node->getId();

				if (nodeId != INVALID_ENTITY)
				{
					// if the entity has a parent node
					bool hasParent = ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeId)->hasEntity(id);

					// if so remove from the node then delete
					if (hasParent)
					{
						ComponentManager<ControllerComponent>::Get().getNodeComponent(nodeId)->removeEntity(id);
						FAY_LOG_DEBUG("Removing entity from: " << nodeId << " Entity Id is: " << id);
						m_utils->SetSelectedEntity(INVALID_ENTITY);
					}
				}
			}
			if (id != INVALID_ENTITY)
			{
				// Deleting an entity from a node should fully remove and delete it from the scene 
				FAY_LOG_DEBUG("Destroying entity");
				m_utils->GetScene()->destroyEntity(id);
				m_utils->SetSelectedEntity(INVALID_ENTITY);
			}
		}

		ImGui::Separator();

		
		if (scene->getObjectCount() > 0 || scene->getNodeCount() > 0)
		{
			if (ImGui::BeginListBox("##Hierarchy", ImVec2(-FLT_MIN, -FLT_MIN)))
			{

				std::unordered_set<EntityID> entitiesInNodes;

				// Pass 1: Gather all entities that belong to nodes.
				for (auto* nd : scene->getNodes())
				{

					auto* controller = ComponentManager<ControllerComponent>::Get().getNodeComponent(nd->getId());

					if (!controller)
						continue;

					for (EntityID id : controller->getEntities())
					{
						entitiesInNodes.insert(id);
					}
				}
				// Pass 2: Draw all nodes.
				for (auto* nd : scene->getNodes())
				{

					ImGuiTreeNodeFlags flags = 0;

					if (m_utils->GetSelectedNode() == nd->getId())
						flags |= ImGuiTreeNodeFlags_Selected;

					// If node has children, show arrow
					auto* node = ComponentManager<ControllerComponent>::Get().getNodeComponent(nd->getId());

					std::vector<EntityID> nodeEnt;
					if (node)
					{
						nodeEnt = node->getEntities();
					}

					if (!nodeEnt.empty())
						flags |= ImGuiTreeNodeFlags_OpenOnArrow;
					else
						flags |= ImGuiTreeNodeFlags_Leaf;

					bool opened = ImGui::TreeNodeEx(
						(void*)(intptr_t)nd->getId(),
						flags,
						"%s %d",
						nd->getName().c_str(),
						nd->getId()
					);

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
						{
							EntityID id = *(const EntityID*)payload->Data;

							auto* controller = ComponentManager<ControllerComponent>::Get().getNodeComponent(nd->getId());

							if (!controller)
							{
								ImGui::EndDragDropTarget();
								//return;
							}

							if (controller->hasEntity(id))
							{
								controller->removeEntity(id);
							}
							else
							{
								for (auto* n : scene->getNodes())
								{
									auto* c = ComponentManager<ControllerComponent>::Get().getNodeComponent(n->getId());

									if (c && c->hasEntity(id))
									{
										c->removeEntity(id);
										break;
									}
								}
								controller->addEntity(id);
							}
						}
						ImGui::EndDragDropTarget();
					}

					// Clicking the node name selects it

					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						if (m_utils->GetSelectedNode() == nd->getId())
							m_utils->SetSelectedNode(INVALID_NODE);
						else
							m_utils->SetSelectedNode(nd->getId());
					}
					// Draw children when expanded

					if (opened)
					{
						for (EntityID id : nodeEnt)
						{
							std::string name;

							if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
							{
								name = "Sprite " + std::to_string(id);
							}
							else if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
							{
								name = "Cube " + std::to_string(id);
							}
							else
							{
								name = "Entity " + std::to_string(id);
							}

							if (ImGui::Selectable(name.c_str(), m_utils->GetSelectedEntity() == id))
							{
								if (m_utils->GetSelectedEntity() == id)
									m_utils->SetSelectedEntity(INVALID_ENTITY);
								else
									m_utils->SetSelectedEntity(id);
							}
							// Drag and drop
							if (ImGui::BeginDragDropSource())
							{

								ImGui::SetDragDropPayload("ENTITY", &id, sizeof(EntityID));
								ImGui::Text("Entity %d", id);
								ImGui::EndDragDropSource();
							}
						}
						ImGui::TreePop();
					}
				}
				for (auto* obj : scene->getObjects())
				{
					// extract entityID from obj
					EntityID id = obj->getId();

					if (entitiesInNodes.contains(id))
						continue;

					std::string name;


					if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
					{
						name = "Sprite " + std::to_string(id);
					}
					else if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
					{
						name = "Cube " + std::to_string(id);
					}
					else
					{
						name = "Entity " + std::to_string(id);
					}

					if (ImGui::Selectable(name.c_str(), m_utils->GetSelectedEntity() == id))
					{
						m_utils->SetSelectedEntity(m_utils->GetSelectedEntity() == id ? INVALID_ENTITY : id);
					}

					// Drag and drop
					if (ImGui::BeginDragDropSource())
					{
						EntityID id = obj->getId();

						ImGui::SetDragDropPayload("ENTITY", &id, sizeof(EntityID));
						ImGui::Text("Entity %d", id);

						ImGui::EndDragDropSource();
					}
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
					{
						EntityID id = *(const EntityID*)payload->Data;
						// Sanity check : If the entity is already in a node, remove it from that node first. to avoid duplicates
						for (auto* n : scene->getNodes())
						{
							auto* c = ComponentManager<ControllerComponent>::Get().getNodeComponent(n->getId());

							if (c && c->hasEntity(id))
							{
								c->removeEntity(id);
								break;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::EndListBox();
			}
		}
		ImGui::End();
		/// ---------- INSPECTOR ---------- ///
		ImGui::Begin("Inspector");

		EntityID entity = m_utils->GetSelectedEntity();
		NodeID node = m_utils->GetSelectedNode(); // TODO: Change node from using EntityID to NodeID that way the two dont get mixed

		if (node != INVALID_NODE && entity == INVALID_ENTITY) // || 0
		{
			ImGui::Text("Node: %d", node);
			ImGui::Separator();

			if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// Script 
				if (ComponentManager<ScriptComponent>::Get().hasNodeComponent(node))
				{
					if (ImGui::Button("Remove Script Component"))
						ComponentManager<ScriptComponent>::Get().removeNodeComponent(node);
				}
				else if (ImGui::Button("Add Script Component"))
				{
					ComponentManager<ScriptComponent>::Get().addNodeComponent(node, ScriptComponent(node));
				}
			}

			ImGui::Separator();
			if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_utils->drawNodeScriptList(node);
			}
		}

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
							if (ImGui::Button("Remove Transform"))
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
#pragma region DrawFolder
	void EditorUI::DrawFolder(const std::filesystem::path& path)
	{
		ImGui::PushID(path.string().c_str());

		const float tileSize = 80.0f;
		const float iconSize = 64.0f;
		
		ImGui::InvisibleButton("FolderTile", ImVec2(tileSize, 90));

		// Single click
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			FAY_LOG_INFO("Folder: " << path.filename().string());

		}
		
		// Double click
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_currentDir = path;
			FAY_LOG_INFO("Opening Folder: " << path.string());

		}
		// Draw icon

		ImVec2 p = ImGui::GetItemRectMin();

		ImGui::SetCursorScreenPos({ p.x + 8, p.y + 8 });
		ImGui::Image((ImTextureID)(intptr_t)TextureManager::getTexture("Folder")->getId(), ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::SetCursorScreenPos({ p.x + 8, p.y + 74 });
		ImGui::TextWrapped("%s", path.filename().string().c_str());

		ImGui::PopID();

	}
#pragma endregion
#pragma region DrawFile
	void EditorUI::DrawFile(const std::filesystem::path& path)
	{

		ImGui::PushID(path.string().c_str());

		constexpr float tileSize = 80.0f;
		constexpr float iconSize = 64.0f;

		// Clickable area
		ImGui::InvisibleButton("FileTile", ImVec2(tileSize, 90));

		// Single click
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			FAY_LOG_INFO("File: " << path.filename().string());
		}

		// Double click
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			std::string extension = path.extension().string();

			if (extension == ".fayScene")
			{
				// Load Scene
				m_utils->LoadScene(path.string().c_str());
			}

			FAY_LOG_INFO("Opening File: " << path.string());
		}


		// Draw icon
		ImVec2 p = ImGui::GetItemRectMin();

		// choose icon by extension
		std::string ext = path.extension().string();

		ImTextureID icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Blank");

		if (ext == ".json")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Json")->getId();
		else if (ext == ".cs")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Script")->getId();
		else if (ext == ".txt")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Txt")->getId();
		else if (ext == ".mp3")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Mp3")->getId();
		else if (ext == ".wav")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Wav")->getId();
		else if (ext == ".png" || ext == ".jpg")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Blank")->getId();
		else if (ext == ".fayScene")
			icon = (ImTextureID)(intptr_t)TextureManager::getTexture("Scene")->getId();

		ImGui::SetCursorScreenPos({ p.x + 8, p.y + 8 });
		ImGui::Image(icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));

		// Draw filename
		std::string filename = path.filename().string();

		ImGui::SetCursorScreenPos({p.x, p.y + 74});
		ImGui::PushTextWrapPos(p.x + tileSize);
		ImGui::TextWrapped("%s", filename.c_str());
		ImGui::PopTextWrapPos();
		ImGui::PopID();
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
				if (ImGui::MenuItem("Save Scene as"))
				{
					m_utils->SetSaveSceneAsReq(true);
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
		if (m_utils->GetSaveSceneAsReq())
		{
			ImGui::OpenPopup("SaveSceneAsPop");
			m_utils->SetSaveSceneAsReq(false);
		}
		if (m_utils->GetShowControlNameBox())
		{
			ImGui::OpenPopup("ControlNodeBox");
			m_utils->SetShowControlNameBox(false);
		}
		if (m_utils->GetShowTagNameBox())
		{
			ImGui::OpenPopup("TagNameBox");
			m_utils->SetShowTagNameBox(false);
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
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
		if (ImGui::BeginPopup("TagNameBox"))
		{
			static char tagName[256] = "Tag";

			ImGui::InputText("Tag Name", tagName, sizeof(tagName));

			if (ImGui::Button("Create"))
			{
				EntityID id = m_utils->GetSelectedEntity();
				if (id != INVALID_ENTITY)
				{
					std::string tag(tagName);
					if (m_utils->GetRenderMode() == RenderMode::MODE_2D)
					{
						auto* entity = ComponentManager<SpriteComponent>::Get().getComponent(id);

						if (entity)
						{
							if (!entity->hasTag(tag))
							{
								// add tag if it's not already in the list
								entity->addTag(tag);
								ImGui::CloseCurrentPopup();
							}
						}
					}
					else
					{
						auto* entity = ComponentManager<CubeComponent>::Get().getComponent(id);

						if (entity)
						{
							if (!entity->hasTag(tag))
							{
								// add tag if it's not already in the list
								entity->addTag(tag);
								ImGui::CloseCurrentPopup();
							}
						}
					}
				}	
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
		if (ImGui::BeginPopup("ControlNodeBox"))
		{
			static char controlName[256] = "ControlNode";

			ImGui::InputText("Control Node Name", controlName, sizeof(controlName));

			if(ImGui::Button("Create"))
			{
				NodeID id = m_utils->GetFactory()->CreateControllNode(controlName);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("SaveSceneAsPop", nullptr))
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
					MonoDomain* domain = mono_domain_get();
					MonoString* monoSceneName = mono_string_new(domain, file.c_str());
					ScriptGlue::InternalCalls_Scene_SaveSceneAs(monoSceneName);

					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

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
					
					//Seemes to be hanging somewhere in memory possibly the domain is the issue here
					MonoDomain* domain = mono_domain_get();
					MonoString* monoSceneName = mono_string_new(domain, file.c_str());
					
					if (!ScriptGlue::InternalCalls_Scene_LoadScene(monoSceneName))
						FAY_LOG_THROW_ERROR("ScriptGlue::InternalCalls_Scene_LoadScene[EditorUI.cpp-812] Failed to load Scene");

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
#pragma endregion
#pragma region DrawToolsPanel
	void EditorUI::DrawToolsPanel()
	{
		ImGui::Begin("Tools");

		if (ImGui::SliderFloat("Entity Speed", &EditorUtils::s_entitySpeed, 0, 10))
		{
			m_utils->SetEntitySpeed(m_utils->GetEntitySpeed());
		}

		if (ImGui::Button("Play"))
		{
			// This button will act as a toggle state if the state is playing it will invoke m_core->handleScriptExecution() to run scripts if not no scripts will invoke or run
			if (ScriptEngine::BuildAndLoadCoreAssembly())
			{
				m_utils->SetIsPlaying(true);
				FAY_LOG_DEBUG("Scene playing");
			}
			else
			{
				FAY_LOG_ERROR("Failed to build and load core assembly");
			}
		
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_utils->SetIsPlaying(false);

			ResetScriptState();

			ScriptEngine::DestroyScriptDomain();

			FAY_LOG_DEBUG("Scene stopped");

		}
		ImGui::End();
	}
#pragma endregion
#pragma region DrawContentBrowser
	void EditorUI::DrawContentBrowser()
	{
		ImGui::Begin("Content Browser");
		
		std::vector<std::filesystem::path> parts;

		for (const auto& p : m_currentDir)
			parts.push_back(p);

		std::filesystem::path accumulated;

		for (size_t i = 0; i < parts.size(); i++)
		{
			accumulated /= parts[i];

			if (ImGui::SmallButton(parts[i].string().c_str()))
			{
				m_currentDir = accumulated;
			}

			if (i + 1 < parts.size())
			{
				ImGui::SameLine();
				ImGui::TextUnformatted(">");
				ImGui::SameLine();
			}
		}

		ImGui::Separator();

		constexpr float tileSize = 80.0f;
		constexpr float padding = 16.0f;

		float cellSize = tileSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, nullptr, false);

		for (const auto& entry : std::filesystem::directory_iterator(m_currentDir))
		{
			const auto& path = entry.path();

			if (entry.is_directory())
				DrawFolder(path);
			else
				DrawFile(path);
			
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
#pragma endregion 
#pragma region DrawCodeEditor
	void EditorUI::DrawCodeEditor()
	{
		ImGui::Begin("Code Editor");

		static std::string my_code_buffer = "#include <iostream>\n\nint main() {\n    std::cout << \"Hello World!\";\n    return 0;\n}";
		SetupCodeEditor("##my_editor", my_code_buffer);
		ImGui::End();
	}
#pragma endregion 
#pragma region InputTextCallback
	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			std::string* str = static_cast<std::string*>(data->UserData);

			IM_ASSERT(data->Buf == str->c_str());

			str->resize(data->BufTextLen);
			data->Buf = str->data();
		}

		return 0;
	}
#pragma endregion
#pragma region SetupCodeEditor
	void EditorUI::SetupCodeEditor(const char* label, std::string& text_buffer)
	{
		int line_count = 1;
		
		for (char c : text_buffer)
		{
			if (c == '\n') line_count++;
		}

		if (ImGui::BeginTable("##editor_layout", 2, ImGuiTableFlags_None, ImVec2(-1.0f, -1.0f)))
		{
			// Setup fixed with for line numbers, and strectching width for the input box
			float line_number_width = ImGui::CalcTextSize("9999").x + ImGui::GetStyle().FramePadding.x * 2;
			ImGui::TableSetupColumn("line_numbers", ImGuiTableColumnFlags_WidthFixed, line_number_width);
			ImGui::TableSetupColumn("text_input", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			// --- COLUMN 1 : LINE NUMBERS --- 
			ImGui::TableNextColumn();

			// Create a child window for line numbers so it can scroll indepently/synced
			// We turn off the scrollbar visibly so it looks like part of the editor
			ImGuiWindowFlags child_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::BeginChild("##line_nums_child", ImVec2(0, -1.0f), ImGuiChildFlags_None, child_flags);

			// Match the exact top-padding of the InputTextMultiline box
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);

			// Generate the vertical stack of line numbers
			for (int i = 1; i <= line_count; i++)
			{
				ImGui::TextDisabled("%d", i); // Dimmed color for line numbers
			}

			// Store this child window pointer so we can force scroll sync later
			ImGuiWindow* line_num_window = ImGui::GetCurrentWindow();
			ImGui::EndChild();

			// --- COLUMN 2 : TEXT INPUT ---
			ImGui::TableNextColumn();

			// Use custom style overrides to remove the left border ofthe input field
			// so it visiually blends with the line-number column perfectly
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

			// Force Tab keys to type real tabs inside the text box instead of switching focus
			ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_AllowTabInput;

			//std::string& text_buffer; is the parameter used in the multiline right now
			ImGui::InputTextMultiline(label, text_buffer.data(), text_buffer.capacity() + 1, ImVec2(-1.0f, -1.0f), input_flags | ImGuiInputTextFlags_CallbackResize, InputTextCallback, &text_buffer);

			// Capture the scroll position of the input box right after rendering it
			ImGuiWindow* input_window = ImGui::GetCurrentWindow();
			float current_scroll_y = ImGui::GetScrollY();

			ImGui::PopStyleVar();

			// --- CRITICAL STEP: SYNCRONIZE SCROLLING --- 
			// Force the line number window to match the exact vertical scroll of the text editor
			if (line_num_window && input_window)
			{
				line_num_window->Scroll.y = current_scroll_y;
			}
			ImGui::EndTable();
		}
	}
#pragma endregion
#pragma region ResetScriptState
	void EditorUI::ResetScriptState()
	{
		auto& scriptEntities = ComponentManager<ScriptComponent>::Get().getEntities();

		auto& scriptNodes = ComponentManager<ScriptComponent>::Get().getNodes();
	
		// Reset entity scripts
		for (EntityID e : scriptEntities)
		{
			ScriptComponent* comp = ComponentManager<ScriptComponent>::Get().getComponent(e);

			if (!comp)
				continue;

			for (auto& script : comp->scripts)
			{
				script.hasStarted = false;
			}
		}

		// Reset node scripts
		for (NodeID n : scriptNodes)
		{
			ScriptComponent* comp = ComponentManager<ScriptComponent>::Get().getNodeComponent(n);

			if (!comp)
				continue;

			for (auto& script : comp->scripts)
			{
				script.hasStarted = false;
			}
		}
	}
#pragma endregion
}