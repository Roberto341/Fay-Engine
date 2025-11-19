#include <EngineEditor/Editor.h>
#include <cmath>
namespace Fay
{
	EntityID Editor::s_SelectedEntity = 0;
	SceneType Editor::s_ActiveScene = SceneType::None; // set this back to none
	bool Editor::shouldRefreshScenes = false;
	bool Editor::shouldRefreshConfigs = false;
	bool Editor::shouldShowViewport = true; // show viewport as default
	bool Editor::newSceneRequested = false;
	Editor::Editor(Window& window) : m_window(window), m_framebuffer(window.getWidth(), window.getHeight())
	{
		// Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_window.getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		m_camera3D = new Camera3D(Vec3(0, 0, 5), Vec3(0, 0, -1), Vec3(0, 1.0f, 0));
		//camera(Fay::Vec3(0, 0, 5), Fay::Vec3(0, 0, -1), Fay::Vec3(0, 1.0f, 0));
		m_shader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
		Shader& shader = *m_shader;
		
		m_batchRenderer = new BatchRenderer();
		m_renderLayer = new TileLayer(m_batchRenderer);
		m_renderLayer->setShader(m_shader);
		m_tileRenerLayer = new TileLayer(m_batchRenderer);
		m_Scene.setSceneType(SceneType::Scene2D);
		m_lastTime = glfwGetTime();
		selectedEntityID = -1;
		m_selectedTile = TileInfo();
		m_tiles.resize(m_window.getWidth() / 32 * m_window.getHeight() / 32, TileInfo());
	}

	Editor::~Editor()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::cameraUpdate()
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - m_lastTime;
		m_lastTime = currentTime;

		float speed = 5.0f;
		if (m_window.isKeyPressed(GLFW_KEY_W))
		{
			auto delta = m_camera3D->getForward() * speed * deltaTime;
			m_camera3D->move(delta);
		}
		if (m_window.isKeyPressed(GLFW_KEY_S))
		{
			auto delta = m_camera3D->getForward() * -speed * deltaTime;
			m_camera3D->move(delta);
		}
		Vec3 right = m_camera3D->getForward().cross(Vec3(0, 1, 0)).normalized();
		if (m_window.isKeyPressed(GLFW_KEY_RIGHT))
			m_camera3D->move(right * speed * deltaTime);
		if (m_window.isKeyPressed(GLFW_KEY_LEFT))
			m_camera3D->move(right * -speed * deltaTime);

		Vec3 up = Vec3(0, 1, 0);
		if (m_window.isKeyPressed(GLFW_KEY_UP))
			m_camera3D->move(up * speed * deltaTime);
		if (m_window.isKeyPressed(GLFW_KEY_DOWN))
			m_camera3D->move(up * -speed * deltaTime);
	}
	void Editor::run()
	{
		while (!m_window.closed())
		{
			//m_window.clear();
			glfwPollEvents();

			Fay::ScriptGlue::SetWindow(m_window); // for input
			// Find a way to invoke this in runtime
			ScriptEngine::InvokeStatic("EntityScript", "OnUpdate");

			//cameraUpdate();
			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			// Render scene into framebuffer
			m_framebuffer.bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// new render mode
			m_Scene.render(m_renderLayer);
			//m_framebuffer.unbind(m_window.getWidth(), m_window.getHeight()); // produces garbage value for tile map 
			m_framebuffer.unbind(); // try this instead

			// Dockspace
			setupDockspace();
			// Toggle between pages
			if (shouldShowViewport)
			{
				// Viewport window
				setupViewport();   // Shows 2D/3D viewport + entity editor
				setupEntityPanel();
			}
			else
			{
				setupTileMap();    // Shows tilemap or palette editor
				setupTilePalette();
			}
			setupFileMenu();
			setupWireFrame();
			// Rendering ImGui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Multi-viewport
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup);
			}

			m_window.update();
		}
	}

	void Editor::SetSelectedEntity(EntityID id)
	{
		s_SelectedEntity = id;
	}

	void Editor::SetActiveScene(SceneType type)
	{
		s_ActiveScene = type;
	}

	EntityID Editor::GetSelEntity()
	{
		return s_SelectedEntity;
	}

	bool Editor::IsSceneActive()
	{
		return s_ActiveScene == SceneType::Scene2D || s_ActiveScene == SceneType::Scene3D;
	}

	SceneType Editor::GetCurrentScene()
	{
		return s_ActiveScene;
	}

	void Editor::setupDockspace()
	{
		// Get the main viewport
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
				if (ImGui::MenuItem("New Scene", "Ctrl+S"))
				{
					newSceneRequested = true;
				}
				if (ImGui::MenuItem("Open Scene")) {}
				if (ImGui::MenuItem("Save Scene")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Undo", "Ctrl+Z");
				ImGui::MenuItem("Redo", "Ctrl+Y");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Show Viewport"))
				{
					shouldShowViewport = true;

				}
				if (ImGui::MenuItem("Show Tile Map"))
				{
					shouldShowViewport = false;
				}
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
		if (newSceneRequested)
		{
			ImGui::OpenPopup("NewSceneTPop");
			newSceneRequested = false;
		}

		if (ImGui::BeginPopupModal("NewSceneTPop", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char sceneName[256] = "Untitled";

			ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));

			if (ImGui::Button("Create"))
			{
				std::string filename = std::string(sceneName) + ".fayScene";
				std::string fullPath = "Res/Scenes/" + filename;
				createScene(fullPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
	void Editor::loadScene(const std::string& path)
	{
		if (path.ends_with(".fayScene"))
		{
			if (m_Scene.loadScene(path, m_textureManager))
			{
				FAY_LOG_INFO("FayScene Loaded: " << path);
				FAY_LOG_INFO("Object count: " << m_Scene.getObjects().size());

				if (m_renderMode == RenderMode::MODE_2D)
				{
					SetActiveScene(SceneType::Scene2D);
					m_Scene.setSceneType(SceneType::Scene2D);
				}
				else if (m_renderMode == RenderMode::MODE_3D)
				{
					SetActiveScene(SceneType::Scene3D);
					m_Scene.setSceneType(SceneType::Scene3D);
				}

				m_renderLayer->clear();

				// Just add the objects to the render layer, no ECS mutation
				for (auto obj : m_Scene.getObjects())
				{
					m_renderLayer->add(obj);
				}

				m_currentScenePath = path;
			}
			else {
				FAY_LOG_ERROR("Failed to load FayScene: " << path);
			}
		}

	}
	void Editor::saveScene(const std::string& path)
	{
		if (path.ends_with(".fayScene"))
		{
			if (m_Scene.saveScene(path))
				FAY_LOG_INFO("FayScene Saved: " << path)
			else
				FAY_LOG_ERROR("Failed to save FayScene" << path);
		}
		else
		{
			FAY_LOG_WARN("Failed to save, Unkown Scene type: " << path);
		}
	}

	void Editor::saveCurrentScene()
	{
		if (m_currentScenePath.empty())
		{
			FAY_LOG_WARN("No Scene currently loaded to save.");
			return;
		}

		if (m_currentScenePath.ends_with(".fayScene"))
		{
			saveScene(m_currentScenePath);
		}
		else
		{
			FAY_LOG_WARN("Failed to save, Unkown Scene type: " << m_currentScenePath);
		}
	}

	void Editor::createScene(const std::string& path)
	{
		m_Scene.clear();
		m_renderLayer->clear();
		m_Scene.saveScene(path);
		m_Scene.setSceneType((m_renderMode == RenderMode::MODE_2D) ? SceneType::Scene2D : SceneType::Scene3D);
		m_currentScenePath = path;
		FAY_LOG_INFO("New Scene Created: " << path);
		shouldRefreshScenes = true;
	}

	void Editor::setupFileMenu()
	{
		ImGui::Begin("File");
		// configuration file load and save 
		if (!shouldShowViewport)
			setConfigBox();
		else
		{
			setupRenderMode();
			setSceneBox();
		}
		ImGui::End();
	}
	void Editor::setupViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		// --- Get viewport and mouse info ---
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 viewportPos = ImGui::GetWindowPos();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 mouse = ImGui::GetMousePos();

		// Draw framebuffer to viewport
		ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImVec2 imgPos = ImGui::GetItemRectMin();
		ImVec2 imgSize = ImGui::GetItemRectSize();

		// --- Convert mouse to viewport-local coordinates ---
		Vec2 relativeMousePos = {
			mouse.x - imgPos.x,
			(imgPos.y + viewportSize.y) - mouse.y
		};
		Vec2 worldMousePos = {
			relativeMousePos.x - (viewportSize.x / 2.0f),
			relativeMousePos.y - (viewportSize.y / 2.0f)
		};

		bool hoveredViewport = ImGui::IsItemHovered();
		bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
		bool clickedLeft = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		if (hoveredViewport && clickedLeft && !gizmoActive)
			selectedEntityID = -1;

		Mat4 modelMatrix = Mat4::identity();
		Mat4 viewMatrix = Mat4::identity();
		Mat4 projMatrix = Mat4::identity();

		// --- 2D Mode ---
		if (m_renderMode == RenderMode::MODE_2D)
		{
			// reallocates color & depth attachments
			if ((int)viewportSize.x != m_framebuffer.getWidth() || (int)viewportSize.y != m_framebuffer.getHeight())
				m_framebuffer.resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

			float halfW = viewportSize.x * 0.5f;
			float halfH = viewportSize.y * 0.5f;
			projMatrix = Mat4::orthographic(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
			viewMatrix = Mat4::identity();

			m_shader->enable();
			m_shader->setUniformMat4("pr_matrix", projMatrix);
			m_shader->setUniformMat4("vw_matrix", viewMatrix);

			ImGui::InvisibleButton("viewport_btn", viewportSize);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetRect(imgPos.x, imgPos.y, imgSize.x, imgSize.y);

			// --- Selected sprite gizmo ---
			if (selectedEntityID != -1)
			{
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(selectedEntityID);
				if (comp && comp->sprite)
				{
					auto* sprite = comp->sprite;
					Vec3 pos = sprite->getPosition();
					Vec3 size = sprite->getSize();
					Mat4 model = Mat4::translation(pos) * Mat4::scale(Vec3(size.x, size.y, 1.0f));
					m_shader->setUniformMat4("ml_matrix", model);

					bool manipulated = ImGuizmo::Manipulate(viewMatrix.data(), projMatrix.data(),
						ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, model.data());

					if (manipulated)
					{
						Vec3 newPos(model.elements[12], model.elements[13], model.elements[14]);
						sprite->setPosition(newPos);
					}

					// Debug draw bounding box
					Vec3 corners[4] = { Vec3(0,0,0), Vec3(1,0,0), Vec3(1,1,0), Vec3(0,1,0) };
					ImVec2 screenCorners[4];
					const float fbW = imgSize.x;
					const float fbH = imgSize.y;
					bool valid = true;

					for (int i = 0; i < 4; ++i)
					{
						Vec4 world = model * Vec4(corners[i].x, corners[i].y, corners[i].z, 1.0f);
						Vec4 clip = projMatrix * viewMatrix * world;
						if (clip.w == 0.0f) { valid = false; break; }
						clip.x /= clip.w; clip.y /= clip.w;
						float ndcX = clip.x * 0.5f + 0.5f;
						float ndcY = clip.y * 0.5f + 0.5f;
						screenCorners[i] = ImVec2(imgPos.x + ndcX * fbW, imgPos.y + (1.0f - ndcY) * fbH);
					}

					if (valid)
					{
						auto* dl = ImGui::GetWindowDrawList();
						dl->AddPolyline(screenCorners, 4, IM_COL32(255, 255, 0, 155), true, 2.0f);
					}
				}
			}

			// --- Sprite selection ---
			auto& spriteEntities = ComponentManager<SpriteComponent>::Get().getEntities();
			int idx = 0;
			for (auto* sp : m_Scene.getObjects())
			{
				Vec3 pos = sp->getPosition();
				Vec3 size = sp->getSize();
				if (worldMousePos.x >= pos.x && worldMousePos.x <= pos.x + size.x &&
					worldMousePos.y >= pos.y && worldMousePos.y <= pos.y + size.y &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					selectedEntityID = spriteEntities[idx];
					SetSelectedEntity(selectedEntityID);
					break;
				}
				idx++;
			}
		}

		// --- 3D Mode ---
		else if (m_renderMode == RenderMode::MODE_3D)
		{
			if ((int)viewportSize.x != m_framebuffer.getWidth() || (int)viewportSize.y != m_framebuffer.getHeight())
				m_framebuffer.resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

			float aspect = viewportSize.x / viewportSize.y;
			float fov = 70.0f;
			float near = 0.1f;
			float far = 1000.0f;

			Mat4 proj = Mat4::perspective(fov, aspect, near, far);
			Mat4 view = m_camera3D->getViewMatrix();

			m_shader->enable();
			m_shader->setUniformMat4("pr_matrix", proj);
			m_shader->setUniformMat4("vw_matrix", view);
			m_shader->setUniformMat4("ml_matrix", modelMatrix);

			ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
			ImVec2 imgPos = ImGui::GetItemRectMin();
			ImVec2 imgSize = ImGui::GetItemRectSize();
			ImGui::InvisibleButton("viewport_btn", viewportSize, ImGuiButtonFlags_MouseButtonLeft);

			// Selected cube gizmo
			if (selectedEntityID != -1)
			{
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(selectedEntityID);
				if (comp && comp->cube)
				{
					auto* cube = comp->cube;
					Mat4 model = Mat4::translation(cube->getPosition()) * Mat4::scale(cube->getSize());
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetOrthographic(false);

					bool manip = ImGuizmo::Manipulate(view.data(), proj.data(), ImGuizmo::TRANSLATE, ImGuizmo::WORLD, model.data());
					if (manip)
					{
						cube->setPosition(Vec3(model.elements[12], model.elements[13], model.elements[14]));
					}
				}
			}

			// Raycast selection
			Ray ray = getRayFromMouse(mouse, viewportPos, viewportSize, proj, view);
			float closestT = FLT_MAX;
			int selectedIdx = -1;
			for (int i = 0; i < m_Scene.getObjects().size(); ++i)
			{
				auto* cube = m_Scene.getObjects()[i];
				Vec3 aabbMin = cube->getPosition() - cube->getSize() * 0.5f;
				Vec3 aabbMax = cube->getPosition() + cube->getSize() * 0.5f;
				float t;
				if (intersectRayAABB(ray.origin, ray.dir, aabbMin, aabbMax, t))
				{
					if (t > 0 && t < closestT) { closestT = t; selectedIdx = i; }
				}
			}

			bool mouseInViewport =
				mouse.x >= viewportPos.x && mouse.x <= viewportPos.x + viewportSize.x &&
				mouse.y >= viewportPos.y && mouse.y <= viewportPos.y + viewportSize.y;

			if (mouseInViewport && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				if (selectedIdx != -1)
				{
					auto* selectedObj = m_Scene.getObjects()[selectedIdx];
					auto& cubeEntities = ComponentManager<CubeComponent>::Get().getEntities();
					for (EntityID e : cubeEntities)
					{
						CubeComponent* comp = ComponentManager<CubeComponent>::Get().getComponent(e);
						if (comp && comp->cube == selectedObj)
						{
							selectedEntityID = e;
							SetSelectedEntity(e);
							break;
						}
					}
				}
				else selectedEntityID = -1;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		// --- Entities panel ---
		ImGui::Begin("Entities");
		if (m_renderMode == RenderMode::MODE_2D)
		{
			if (ImGui::Button("Add Sprite"))
			{
				EntityID entity = m_Scene.getNextId();
				auto sprite = new Sprite(entity, 0, 0, 0, 100, 100, 0, Vec4(1, 0, 0, 1));
				ComponentManager<SpriteComponent>::Get().addComponent(entity, SpriteComponent(sprite));
				m_Scene.addObject(sprite);
			}

			if (ImGui::Button("Delete Sprite") && selectedEntityID != -1)
			{
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(selectedEntityID);
				if (comp && comp->sprite)
				{
					m_renderLayer->remove(comp->sprite);
					m_Scene.removeObject(comp->sprite);
					ComponentManager<SpriteComponent>::Get().removeComponent(selectedEntityID);
					selectedEntityID = -1;
				}
			}
		}
		else if (m_renderMode == RenderMode::MODE_3D)
		{
			if (ImGui::Button("Add Cube"))
			{
				EntityID entity = m_Scene.getNextId();
				auto* cube = new Cube(entity, 0, 0, 0, 1, 1, 1, Vec4(1, 0, 0, 1));
				ComponentManager<CubeComponent>::Get().addComponent(entity, CubeComponent(cube));
				m_Scene.addObject(cube);
			}

			if (ImGui::Button("Delete Cube") && selectedEntityID != -1)
			{
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(selectedEntityID);
				if (comp && comp->cube)
				{
					m_renderLayer->remove(comp->cube);
					m_Scene.removeObject(comp->cube);
					ComponentManager<CubeComponent>::Get().removeComponent(selectedEntityID);
					selectedEntityID = -1;
				}
			}
		}

		ImGui::End();
	}

	void Editor::setupTileMap()
	{
		ImGui::Begin("Tile map", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		// --- Get viewport and mouse info ---

		const int tileSize = 32;
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		int visibleTilesX = static_cast<int>(contentSize.x / tileSize);
		int visibleTilesY = static_cast<int>(contentSize.y / tileSize);
		ImVec2 canvasSize(visibleTilesX * tileSize, visibleTilesY * tileSize);

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_p1(canvas_p0.x + canvasSize.x, canvas_p0.y + canvasSize.y);

		ImGui::InvisibleButton("canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(30, 30, 30, 255));

		// Draw grid
		for (int x = 0; x < visibleTilesX; x++)
			drawList->AddLine(ImVec2(canvas_p0.x + x * tileSize, canvas_p0.y),
				ImVec2(canvas_p0.x + x * tileSize, canvas_p1.y), IM_COL32(100, 100, 100, 255));
		for (int y = 0; y < visibleTilesY; y++)
			drawList->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y * tileSize),
				ImVec2(canvas_p1.x, canvas_p0.y + y * tileSize), IM_COL32(100, 100, 100, 255));

		// Draw tiles
		for (int y = 0; y < visibleTilesY; y++)
		{
			for (int x = 0; x < visibleTilesX; x++)
			{
				TileInfo tile = getTile(x, y);
				if (tile.id == 0) continue;

				ImVec2 min(canvas_p0.x + x * tileSize, canvas_p0.y + y * tileSize);
				ImVec2 max(min.x + tileSize, min.y + tileSize);

				if (tile.isTexture && tile.texture)
					drawList->AddImage((ImTextureID)(intptr_t)tile.texture->getId(), min, max, ImVec2(0, 1), ImVec2(1, 0));
				else
				{
					Vec4 c = tile.color;
					drawList->AddRectFilled(min, max,
						IM_COL32((int)(c.x * 255), (int)(c.y * 255), (int)(c.z * 255), (int)(c.w * 255)));
				}
			}
		}

		// Mouse paint
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			ImVec2 local(mousePos.x - canvas_p0.x, mousePos.y - canvas_p0.y);
			int tileX = static_cast<int>(local.x / tileSize);
			int tileY = static_cast<int>(local.y / tileSize);

			if (tileX >= 0 && tileX < visibleTilesX && tileY >= 0 && tileY < visibleTilesY)
			{
				TileInfo tile = m_selectedTile;
				float halfW = m_framebuffer.getWidth() / 2.0f;
				float halfH = m_framebuffer.getHeight() / 2.0f;
				tile.position = Vec3(-halfW + tileX * tileSize, halfH - tileY * tileSize, 0);
				tile.size = Vec3(tileSize, tileSize, 1);
				setTile(tileX, tileY, tile);
			}
		}

		// Hover highlight
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 hover(mousePos.x - canvas_p0.x, mousePos.y - canvas_p0.y);
		int hoverX = static_cast<int>(hover.x / tileSize);
		int hoverY = static_cast<int>(hover.y / tileSize);
		if (hoverX >= 0 && hoverX < visibleTilesX && hoverY >= 0 && hoverY < visibleTilesY)
		{
			ImVec2 min(canvas_p0.x + hoverX * tileSize, canvas_p0.y + hoverY * tileSize);
			ImVec2 max(min.x + tileSize, min.y + tileSize);
			drawList->AddRect(min, max, IM_COL32(255, 255, 0, 200), 0.0f, 0, 2.0f);
		}

		ImGui::End();
	}
	void Editor::setupTilePalette()
	{
		ImGui::Begin("Tile palette", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		if (!m_currentConfigPath.empty())
		{
			showPalette();
			createTile();
		}
		else {
			ImGui::Text("No Configuration is loaded please load one");
		}
		ImGui::End();
	}
	void Editor::setupRenderMode()
	{
		const char* modes[] = { "2D", "3D" };
		static int currentMode = 0;
		if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
		{
			if (m_Scene.canSwitchScene())
			{
				m_renderMode = (RenderMode)currentMode;
			}
			if (m_renderMode == RenderMode::MODE_3D)
			{
				SetActiveScene(SceneType::Scene3D);
				m_Scene.setSceneType(SceneType::Scene3D);
				m_batchRenderer->setDimension(RenderDimension::D3);
				m_renderLayer->setProjectionType(ProjectionType::Cube3D);
				m_renderLayer->setShader(m_shader);
			}
			if (m_renderMode == RenderMode::MODE_2D)
			{
				SetActiveScene(SceneType::Scene2D);
				m_Scene.setSceneType(SceneType::Scene2D);
				m_batchRenderer->setDimension(RenderDimension::D2);
				m_renderLayer->setProjectionType(ProjectionType::Quad2D);
				m_renderLayer->setShader(m_shader);
			}
		}
	}
	void Editor::setupEntityPanel() const
	{
		ImGui::Begin("Entity Components");
		if (m_renderMode == RenderMode::MODE_2D)
		{
			if (selectedEntityID != -1)
			{
				EntityID entity = selectedEntityID;
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity);

				if (!comp)
					return;
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
			}
		}
		else if (m_renderMode == RenderMode::MODE_3D)
		{
			if (selectedEntityID != -1)
			{
				EntityID entity = selectedEntityID;
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
		ImGui::Begin("Entity Properites");
		if (m_renderMode == RenderMode::MODE_2D)
		{
			if (selectedEntityID != -1)
			{
				EntityID entity = selectedEntityID;
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity);

				if (!comp) return;

				auto* sprite = comp->sprite;

				float color[4] = {
					sprite->getColor().x,
					sprite->getColor().y,
					sprite->getColor().z,
					sprite->getColor().w,
				};

				char idBuf[32];
				sprintf(idBuf, "%d", selectedEntityID);

				ImGui::InputText("Entity ID", idBuf, IM_ARRAYSIZE(idBuf), ImGuiInputTextFlags_ReadOnly);
				if (ImGui::ColorEdit4("Color", color))
				{
					sprite->setColor(Vec4(color[0], color[1], color[2], color[3]));
				}
				if (comp)
					comp->setColor(sprite->getColor());

				float uniformScale = sprite->getSize().x;
				if (ImGui::SliderFloat("Scale", &uniformScale, 0.1f, 500.0f))
				{
					sprite->setSize(Vec3(uniformScale, uniformScale, 0));
				}
				if (comp)
					comp->setSize(sprite->getSize());

				float spriteX = sprite->getPosition().x;
				float spriteY = sprite->getPosition().y;

				if (ImGui::SliderFloat("X Position", &spriteX, -1000.0f, 1000.0f))
				{
					sprite->setPosition(Vec3(spriteX, sprite->getPosition().y, sprite->getPosition().z));
				}
				if (ImGui::SliderFloat("Y Position", &spriteY, -1000.0f, 1000.0f))
				{
					sprite->setPosition(Vec3(sprite->getPosition().x, spriteY, sprite->getPosition().z));

				}
				if (comp)
					comp->setPosition(sprite->getPosition());
			}
		}
		else if (m_renderMode == RenderMode::MODE_3D)
		{
			if (selectedEntityID != -1)
			{
				EntityID entity = selectedEntityID;
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(entity);
				if (!comp) return;

				auto* cube = comp->cube;
				char idBuf[32];
				sprintf(idBuf, "%d", selectedEntityID);

				ImGui::InputText("Entity ID", idBuf, IM_ARRAYSIZE(idBuf), ImGuiInputTextFlags_ReadOnly);
				float color[4] = {
					cube->getColor().x,
					cube->getColor().y,
					cube->getColor().z,
					cube->getColor().w,
				};

				if (ImGui::ColorEdit4("Color", color))
				{
					cube->setColor(Vec4(color[0], color[1], color[2], color[3]));
				}
				if (comp)
					comp->setColor(cube->getColor());
				float uniformScale = cube->getSize().x;
				if (ImGui::SliderFloat("Scale", &uniformScale, 0.1f, 500.0f))
				{
					cube->setSize(Vec3(uniformScale, uniformScale, uniformScale));
				}
				if (comp)
					comp->setSize(cube->getSize());

				float cubeX = cube->getPosition().x;
				float cubeY = cube->getPosition().y;
				float cubeZ = cube->getPosition().z;

				if (ImGui::SliderFloat("X Position", &cubeX, -1000.0f, 1000.0f))
				{
					cube->setPosition(Vec3(cubeX, cube->getPosition().y, cube->getPosition().z));
				}
				if (ImGui::SliderFloat("Y Position", &cubeY, -1000.0f, 1000.0f))
				{
					cube->setPosition(Vec3(cube->getPosition().x, cubeY, cube->getPosition().z));

				}
				if (ImGui::SliderFloat("Z Position", &cubeZ, -1000.0f, 1000.0f))
				{
					cube->setPosition(Vec3(cube->getPosition().x, cube->getPosition().y, cubeZ));
				}
				if (comp)
					comp->setPosition(cube->getPosition());
			}
		}
		ImGui::End();
	}
	void Editor::setupWireFrame() const
	{
		ImGui::Begin("Properties");
		static bool viewWire = false;
		if (ImGui::Checkbox("Wireframe", &viewWire))
		{
			if (viewWire)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
		ImGui::End();
	}
	void Editor::setSceneBox()
	{
		static std::string sceneDir = "Res\\Assets\\Scenes\\";
		// 2d
		static std::vector<std::string> scenes;
		shouldRefreshScenes = true;
		static int selectedSceneIndex = -1;
		if (shouldRefreshScenes) {

			scenes = m_Scene.listScenesDir(sceneDir);
			selectedSceneIndex = -1;
			shouldRefreshScenes = false;
		}
		ImGui::Text("Select scene");

		if (ImGui::BeginListBox("Scenes"))
		{
			for (int i = 0; i < scenes.size(); i++)
			{
				bool isSelected = (selectedSceneIndex == i);
				if (ImGui::Selectable(scenes[i].c_str(), isSelected))
				{
					selectedSceneIndex = i;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (selectedSceneIndex >= 0)
		{
			std::string fullPath = sceneDir + scenes[selectedSceneIndex];
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				loadScene(fullPath);  // Double-click loads the scene
			}
			if (ImGui::Button("Delete Scene"))
			{
				if (m_Scene.deleteSceneFile(fullPath))
				{
					FAY_LOG_WARN("Deleted Scene: " << scenes[selectedSceneIndex]);
					scenes = m_Scene.listScenesDir(sceneDir);
					selectedSceneIndex = 0;
					m_Scene.clear();
				}
				else {
					FAY_LOG_ERROR("Failed to delete FayScene: " << scenes[selectedSceneIndex]);
				}
				shouldRefreshScenes = true;
			}
		}
		if (ImGui::Button("Save Scene"))
		{
			saveCurrentScene();
		}
		// Create new scene
		if (ImGui::Button("New Scene"))
		{
			ImGui::OpenPopup("NewScenePop");
		}
		if (ImGui::BeginPopup("NewScenePop"))
		{
			static char sceneName[256] = "Untitled";

			ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));

			if (ImGui::Button("Create"))
			{
				std::string filename = std::string(sceneName) + ".fayScene";
				std::string fullPath = "Res/Scenes/" + filename;
				createScene(fullPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void Editor::setConfigBox()
	{
		static std::string configDir = "Res\\Assets\\Configurations\\";
		static std::vector<std::string> configs;
		static int selectedConfigIndex = -1;
		static bool shouldRefreshConfigs = true; // Make sure this is properly defined somewhere

		if (shouldRefreshConfigs)
		{
			configs = m_Configuration.listConfigurationsDir(configDir);
			selectedConfigIndex = -1;
			shouldRefreshConfigs = false;
		}

		ImGui::Text("Select configuration");

		if (ImGui::BeginListBox("Configurations"))
		{
			for (int i = 0; i < configs.size(); i++)
			{
				bool isSel = (selectedConfigIndex == i);
				if (ImGui::Selectable(configs[i].c_str(), isSel))
				{
					selectedConfigIndex = i;
				}

				if (isSel)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (selectedConfigIndex >= 0)
		{
			std::string fullPath = configDir + configs[selectedConfigIndex];

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				loadPalette(fullPath);
			}

			if (ImGui::Button("Delete Configuration"))
			{
				if (m_Configuration.deleteConfiguration(fullPath))
				{
					FAY_LOG_WARN("Deleted Configuration: " << configs[selectedConfigIndex]);
					configs = m_Configuration.listConfigurationsDir(configDir);
					selectedConfigIndex = 0;
					m_Configuration.clear();
					shouldRefreshConfigs = true;
					m_currentConfigPath = "";
				}
				else
				{
					FAY_LOG_ERROR("Failed to delete Configuration: " << configs[selectedConfigIndex]);
				}
				shouldRefreshConfigs = true;
			}
		}
		if (ImGui::Button("Save Configuration"))
		{
			saveCurrentPalette();
		}
		// Create new configuration
		if (ImGui::Button("New Configuration"))
		{
			ImGui::OpenPopup("NewConfigPop");
		}
		if (ImGui::BeginPopup("NewConfigPop"))
		{
			static char configName[256] = "Untitled";

			ImGui::InputText("Config Name", configName, sizeof(configName));

			if (ImGui::Button("Create"))
			{
				std::string filename = std::string(configName) + ".config";
				std::string fullPath = "Res/Assets/Configurations/" + filename;

				createPalette(fullPath);
				shouldRefreshConfigs = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	bool Editor::intersectRayAABB(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& aabbMin, const Vec3& aabbMax, float& t)
	{
		float t1 = (aabbMin.x - rayOrigin.x) / rayDir.x;
		float t2 = (aabbMax.x - rayOrigin.x) / rayDir.x;
		float t3 = (aabbMin.y - rayOrigin.y) / rayDir.y;
		float t4 = (aabbMax.y - rayOrigin.y) / rayDir.y;
		float t5 = (aabbMin.z - rayOrigin.z) / rayDir.z;
		float t6 = (aabbMax.z - rayOrigin.z) / rayDir.z;

		float tmin = std::max({ std::min(t1,t2), std::min(t3,t4), std::min(t5,t6) });
		float tmax = std::min({ std::max(t1,t2), std::max(t3,t4), std::max(t5,t6) });

		if (tmax < 0 || tmin > tmax)
			return false;

		t = (tmin < 0.0f) ? tmax : tmin;
		return true;
	}
	Ray Editor::getRayFromMouse(const ImVec2& mousePos, const ImVec2& viewportPos, const ImVec2& viewportSize, const Mat4& proj, const Mat4& view)
	{
		 // 1. Convert mouse position to normalized device coordinates (-1 to 1)
		float relX = mousePos.x - viewportPos.x;
		float relY = mousePos.y - viewportPos.y;

		float ndcX = (2.0f * relX) / viewportSize.x - 1.0f;
		float ndcY = 1.0f - (2.0f * relY) / viewportSize.y;

		// 2. Define clip-space positions for near and far planes
		Vec4 rayStartClip(ndcX, ndcY, -1.0f, 1.0f);
		Vec4 rayEndClip(ndcX, ndcY, 1.0f, 1.0f);

		// 3. Invert the combined view-projection matrix
		Mat4 invViewProj = (proj * view).inverse();

		// 4. Unproject to world space
		Vec4 rayStartWorld = invViewProj * rayStartClip;
		rayStartWorld /= rayStartWorld.w;

		Vec4 rayEndWorld = invViewProj * rayEndClip;
		rayEndWorld /= rayEndWorld.w;

		// 5. Build ray
		Vec3 origin(rayStartWorld.x, rayStartWorld.y, rayStartWorld.z);
		Vec3 direction = Vec3(
			rayEndWorld.x - rayStartWorld.x,
			rayEndWorld.y - rayStartWorld.y,
			rayEndWorld.z - rayStartWorld.z
		).normalized();

		return { origin, direction };
	}

	TileInfo Editor::getTile(int x, int y) const
	{
		int tilesX = m_window.getWidth() / 32;
		int tilesY = m_window.getHeight() / 32;
		if (x < 0 || x >= tilesX || y < 0 || y >= tilesY)
			return TileInfo();
		return m_tiles[y * tilesX + x];
	}
	void Editor::loadPalette(const std::string& path)
	{
		if (path.ends_with(".config"))
		{
			if (m_Configuration.loadConfiguration(path))
			{
				FAY_LOG_DEBUG("Configuration file loaded: " << path);
				FAY_LOG_DEBUG("Tile count: " << m_Configuration.getSize());
				m_currentConfigPath = path;
			}
		}
		else {
			FAY_LOG_ERROR("Failed to load Configuration: " << path);
		}
	}
	void Editor::savePalette(const std::string& path)
	{
		if (path.ends_with(".config"))
		{
			if (m_Configuration.saveConfiguration(path))
				FAY_LOG_INFO("Configuration Saved: " << path)
			else
				FAY_LOG_ERROR("Failed to save Configuration: " << path);
		}
		else
		{
			FAY_LOG_WARN("Failed to save, Unkown Configuration type: " << path);
		}
	}
	void Editor::createPalette(const std::string& path)
	{
		m_Configuration.clear();
		m_Configuration.saveConfiguration(path);
		m_currentConfigPath = path;
		FAY_LOG_INFO("New Configuration Created: " << path);
		shouldRefreshConfigs = true;
	}
	void Editor::saveCurrentPalette()
	{
		if (m_currentConfigPath.empty())
		{
			FAY_LOG_WARN("No Configuration currently loaded to save.");
			return;
		}

		if (m_currentConfigPath.ends_with(".config"))
		{
			savePalette(m_currentConfigPath);
		}
		else
		{
			FAY_LOG_WARN("Failed to save, Unkown Configuration type: " << m_currentConfigPath);
		}
	}
	void Editor::showPalette()
	{
		static int selectedTileIndex = -1; // no selected tile yet

		float thumbnailSize = 48.0f;
		int itemsPerRow = 8;

		ImGui::Text("Tile Palette");

		for (int i = 0; i < m_Configuration.getSize(); i++)
		{
			//auto& tile = m_Configuration.m_tiles[i]; // Correct data source
			auto& tile = m_Configuration.getTiles()[i];
			ImGui::PushID(i);

			// Clickable area
			if (ImGui::InvisibleButton("tileBtn", ImVec2(thumbnailSize, thumbnailSize)))
			{
				selectedTileIndex = i;
			}

			ImVec2 pos = ImGui::GetItemRectMin();
			ImVec2 size = ImGui::GetItemRectSize();

			// Draw texture or color
			if (tile.isTexture && tile.texture && tile.texture->getId() != 0)
			{
				ImGui::GetWindowDrawList()->AddImage(
					(void*)(intptr_t)tile.texture->getId(),
					pos,
					ImVec2(pos.x + size.x, pos.y + size.y),
					ImVec2(0, 1),
					ImVec2(1, 0)
				);
			}
			else
			{
				ImU32 col = IM_COL32(
					(int)(tile.color.x * 255),
					(int)(tile.color.y * 255),
					(int)(tile.color.z * 255),
					(int)(tile.color.w * 255)
				);
				ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), col);
			}

			// Selection border
			if (i == selectedTileIndex)
			{
				ImGui::GetWindowDrawList()->AddRect(
					pos,
					ImVec2(pos.x + size.x, pos.y + size.y),
					IM_COL32(255, 255, 0, 255),
					0.0f, 0, 2.0f
				);
			}

			// Tooltip
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(tile.name.c_str());

			ImGui::PopID();

			if ((i + 1) % itemsPerRow != 0)
				ImGui::SameLine();
		}

		// Make sure we select from the same source
		if (selectedTileIndex >= 0 && selectedTileIndex < m_Configuration.getSize())
		{
			m_selectedTile = m_Configuration.getTiles()[selectedTileIndex];
		}
	}
	void Editor::createTile()
	{
		ImGui::NewLine();
		ImGui::Separator();
		
		static int tileType = 0; // 0 = Color, 1 = Texture
		static char newTileName[128] = "";
		static char newTexturePath[256] = "";
		static Vec4 newTileColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::Text("Add New Tile");

		// Tile type selector
		ImGui::RadioButton("Color Tile", &tileType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Texture Tile", &tileType, 1);

		ImGui::InputText("Tile Name", newTileName, IM_ARRAYSIZE(newTileName));

		if (tileType == 0)
		{
			ImGui::ColorEdit4("Tile Color", (float*)&newTileColor);
		}
		else
		{
			ImGui::InputText("Texture Path", newTexturePath, IM_ARRAYSIZE(newTexturePath));
		}
		if (ImGui::Button("Add Tile"))
		{
			int nextId = 0;
			if (!m_Configuration.isEmpty())
				nextId = m_Configuration.getNextId();

			if (tileType == 0)
			{
				// Add color tile
				TileInfo tile = TileInfo(nextId, newTileName, newTileColor);
				m_Configuration.addTile(tile);
			}
			else 
			{
				// Add texture tile
				std::string texPath = newTexturePath;
				TileInfo tile = TileInfo(nextId, newTileName, texPath);
				m_Configuration.addTile(tile);
				TextureManager::add(new Texture(newTileName, texPath));
				m_Configuration.getBack().texture = TextureManager::getTexture(newTileName);
			}
			// Reset input fields
			newTileName[0] = '\0';
			newTexturePath[0] = '\0';
			newTileColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (ImGui::Button("Delete Tile"))
		{
			if (m_Configuration.getSize() >= 0)
			{
				// Delete the tile 
				FAY_LOG_DEBUG("Deleting tile: " << m_selectedTile.id);
				m_Configuration.removeTile(m_selectedTile);
			}
		}
		ImGui::Separator();

	}
	void Editor::setTile(int x, int y, const TileInfo& tile)
	{
		int tilesX = m_window.getWidth() / 32;
		int tilesY = m_window.getHeight() / 32;
		m_tiles[y * tilesX + x] = tile;
		// Create a sprite for this tile
		EntityID entity = m_Scene.getNextId();
		Sprite* sprite = nullptr;

		if (tile.isTexture && tile.texture)
			sprite = new Sprite(entity, tile.position.x, tile.position.y, tile.position.z,
				tile.size.x, tile.size.y, tile.size.z, tile.texture);
		else
			sprite = new Sprite(entity, tile.position.x, tile.position.y, tile.position.z,
				tile.size.x, tile.size.y, tile.size.z, tile.color);

		// Add to your scene immediately
		ComponentManager<SpriteComponent>::Get().addComponent(entity, SpriteComponent(sprite));
		m_Scene.addObject(sprite);

		// Store the tile in the array

		FAY_LOG_DEBUG("Added tile at (" << x << "," << y << ") pos="
			<< tile.position.x << "," << tile.position.y << "," << tile.position.z
			<< " sprite=" << sprite);
	}
}