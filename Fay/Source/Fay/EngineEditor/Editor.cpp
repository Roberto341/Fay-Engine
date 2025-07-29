#include <EngineEditor/Editor.h>
#include <cmath>
namespace Fay
{
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

		m_camera3D = new Camera3D(Vec3(0, 0, 5), Vec3(0, 0, -1), Vec3(0, 1.0f, 0));//camera(Fay::Vec3(0, 0, 5), Fay::Vec3(0, 0, -1), Fay::Vec3(0, 1.0f, 0));
		m_shader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag"); //shader's
		m_shader3d = new Shader("Res/Shaders/basic_3d.vert", "Res/Shaders/basic_3d.frag");
		Shader& shader = *m_shader;
		Shader& shader3d = *m_shader3d;
		//m_shader->setUniformMat4("pr_matrix", Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));

		setShader(m_shader);
		setShader3D(m_shader3d);

		m_renderLayer = new TileLayer(&shader, new BatchRenderer());
		m_renderLayer3D = new TileLayer(&shader3d, new BatchRenderer3D());
		selectedSpriteIndex = -1;
		m_lastTime = glfwGetTime();
		selectedCubeIndex = -1;
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
			glfwPollEvents();

			cameraUpdate();
			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			// Render scene into framebuffer
			m_framebuffer.bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// render
			if (m_renderMode == RenderMode::MODE_2D)
			{
				m_renderLayer->clear();
				for (auto* sp : m_scene2D.getObjects())
					m_renderLayer->add(sp);
				m_renderLayer->render();
			}
			else if (m_renderMode == RenderMode::MODE_3D)
			{
				m_renderLayer3D->clear();
				for (auto* c : m_scene3D.getObjects())
					m_renderLayer3D->add(c);
				m_renderLayer3D->render3d();
			}
			m_framebuffer.unbind(m_window.getWidth(), m_window.getHeight());

			// Dockspace
			setupDockspace();
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
			ImGui::Begin("Entity Properites");
			if (m_renderMode == RenderMode::MODE_2D)
			{
				if (selectedSpriteIndex >= 0 && selectedSpriteIndex < (int)m_scene2D.getObjects().size())
				{
					auto* sprite = m_scene2D.getObjects()[selectedSpriteIndex];

					float color[4] = {
						sprite->color.x,
						sprite->color.y,
						sprite->color.z,
						sprite->color.w
					};

					if (ImGui::ColorEdit4("Color", color))
					{
						sprite->color = Vec4(color[0], color[1], color[2], color[3]);
					}

					float uniformScale = sprite->size.x;
					if (ImGui::SliderFloat("Scale", &uniformScale, 0.1f, 500.0f))
					{
						sprite->size.x = uniformScale;
						sprite->size.y = uniformScale;
					}

					float spriteX = sprite->pos.x;
					float spriteY = sprite->pos.y;

					if (ImGui::SliderFloat("X Position", &spriteX, -1000.0f, 1000.0f))
					{
						sprite->pos.x = spriteX;
					}
					if (ImGui::SliderFloat("Y Position", &spriteY, -1000.0f, 1000.0f))
					{
						sprite->pos.y = spriteY;
					}
				}
			}
			else if (m_renderMode == RenderMode::MODE_3D)
			{
				if (selectedCubeIndex >= 0 && selectedCubeIndex < (int)m_scene3D.getObjects().size())
				{
					auto* cube = m_scene3D.getObjects()[selectedCubeIndex];

					float color[4] = {
						cube->color.x,
						cube->color.y,
						cube->color.z,
						cube->color.w
					};

					if (ImGui::ColorEdit4("Color", color))
					{
						cube->color = Vec4(color[0], color[1], color[2], color[3]);
					}

					float uniformScale = cube->size.x;
					if (ImGui::SliderFloat("Scale", &uniformScale, 0.1f, 500.0f))
					{
						cube->size.x = uniformScale;
						cube->size.y = uniformScale;
						cube->size.z = uniformScale;
					}

					float cubeX = cube->pos.x;
					float cubeY = cube->pos.y;
					float cubeZ = cube->pos.z;

					if (ImGui::SliderFloat("X Position", &cubeX, -1000.0f, 1000.0f))
					{
						cube->pos.x = cubeX;
					}
					if (ImGui::SliderFloat("Y Position", &cubeY, -1000.0f, 1000.0f))
					{
						cube->pos.y = cubeY;
					}
					if (ImGui::SliderFloat("Z Position", &cubeZ, -1000.0f, 1000.0f))
					{
						cube->pos.z = cubeZ;
					}
				}
			}
			ImGui::End();
			// Viewport window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			ImVec2 viewportPos = ImGui::GetWindowPos();
			Mat4 modelMatrix = Mat4::translation(Vec3(0, 0, 0));
			Mat4 viewMatrix = Mat4::identity();
			Mat4 proj = Mat4::identity();

			if (m_renderMode == RenderMode::MODE_2D)
			{
				if ((int)viewportSize.x != m_framebuffer.getWidth() || (int)viewportSize.y != m_framebuffer.getHeight())
				{
					m_framebuffer.resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
					float left = -m_framebuffer.getWidth() / 2.0f;
					float right = m_framebuffer.getWidth() / 2.0f;
					float bottom = -m_framebuffer.getHeight() / 2.0f;
					float top = m_framebuffer.getHeight() / 2.0f;
					float near = -1.0f;
					float far = 1.0f;
					proj = Mat4::orthographic(left, right, bottom, top, near, far);
					m_shader->enable();
					m_shader->setUniformMat4("pr_matrix", proj);
					//camera->resize((int)viewportSize.x, (int)viewportSize.y);
				// This should upload proj + view matrices
				}
				ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
				ImVec2 imgPos = ImGui::GetItemRectMin();
				ImVec2 imgSize = ImGui::GetItemRectSize();
				ImGui::InvisibleButton("viewport_btn", viewportSize, ImGuiButtonFlags_MouseButtonLeft);

				ImGuizmo::SetDrawlist();
				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetRect(imgPos.x, imgPos.y, imgSize.x, imgSize.y);
				float left = -m_framebuffer.getWidth() / 2.0f;
				float right = m_framebuffer.getWidth() / 2.0f;
				float bottom = -m_framebuffer.getHeight() / 2.0f;
				float top = m_framebuffer.getHeight() / 2.0f;
				float near = -1.0f;
				float far = 1.0f;

				proj = Mat4::orthographic(-imgSize.x / 2, imgSize.x / 2,
					-imgSize.y / 2, imgSize.y / 2,
					-1, 1);

				m_shader->enable();
				m_shader->setUniformMat4("pr_matrix", proj);
				// Gizmo manipulation for selected sprite
				if (selectedSpriteIndex >= 0 && selectedSpriteIndex < (int)m_scene2D.getObjects().size())
				{
					auto* sprite = m_scene2D.getObjects()[selectedSpriteIndex];

					//Vec3 centerPos = sprite->pos + Vec3(sprite->size.x, sprite->size.y, 0.0f);

					float left = -m_framebuffer.getWidth() / 2.0f;
					float right = m_framebuffer.getWidth() / 2.0f;
					float bottom = -m_framebuffer.getHeight() / 2.0f;
					float top = m_framebuffer.getHeight() / 2.0f;
					float near = -1.0f;
					float far = 1.0f;

					//Mat4 proj = Mat4::orthographic(left, right, bottom, top, near, far);
					// 1. Build model matrix from sprite center

					Mat4 model = Mat4::translation(sprite->pos) *
						Mat4::scale(Vec3(sprite->size.x, sprite->size.y, 1.0f));

					// 2. Setup view and proj (as you're already doing)
					viewMatrix = Mat4::identity(); // if you're using no camera
					proj = Mat4::orthographic(-imgSize.x / 2, imgSize.x / 2,
						-imgSize.y / 2, imgSize.y / 2,
						-1, 1); // your orthographic call

					// 3. Manipulate with ImGuizmo
					bool manipulated = ImGuizmo::Manipulate(
						viewMatrix.data(),
						proj.data(),
						ImGuizmo::TRANSLATE,
						ImGuizmo::LOCAL,
						model.data()
					);

					// 4. Apply new center position directly
					if (manipulated)
					{
						Vec3 newPos(model.elements[12], model.elements[13], model.elements[14]);
						sprite->pos = newPos;
					}

					Vec3 cornersWorld[4] = {
						sprite->pos, // bottom-left
						sprite->pos + Vec3(sprite->size.x, 0, 0), // bottom-right
						sprite->pos + Vec3(sprite->size.x, sprite->size.y, 0), // top-right
						sprite->pos + Vec3(0, sprite->size.y, 0), // top-left
					};

					Mat4 view = viewMatrix;
					//Mat4 proj = camera->getProjection();

					ImVec2 screenCorners[4];
					bool valid = true;

					const float fbWidth = imgSize.x;
					const float fbHeight = imgSize.y;
					//const float fbWidth = (float)m_framebuffer.getWidth();
					//const float fbHeight = (float)m_framebuffer.getHeight();

					Vec3 localCorners[4] = {
						Vec3(0, 0, 0),                          // bottom-left
						Vec3(1, 0, 0),                          // bottom-right
						Vec3(1, 1, 0),                          // top-right
						Vec3(0, 1, 0),                          // top-left
					};


					for (int i = 0; i < 4; ++i)
					{
						Vec4 world = model * Vec4(localCorners[i].x, localCorners[i].y, localCorners[i].z, 1.0f);
						Vec4 clip = proj * view * world;

						if (clip.w == 0.0f) {
							valid = false;
							break;
						}

						clip.x /= clip.w;
						clip.y /= clip.w;

						float ndcX = clip.x * 0.5f + 0.5f;
						float ndcY = clip.y * 0.5f + 0.5f;

						// Flip Y to match ImGui::Image(0,1)-(1,0) flip
						float screenX = imgPos.x + ndcX * fbWidth;
						float screenY = imgPos.y + (1.0f - ndcY) * fbHeight;

						screenCorners[i] = ImVec2(screenX, screenY);
					}

					if (valid)
					{
						auto* dl = ImGui::GetWindowDrawList();
						for (auto& sc : screenCorners) {
							dl->AddCircleFilled(sc, 3.0f, IM_COL32(255, 0, 0, 255));
						}
						dl->AddPolyline(screenCorners, 4, IM_COL32(255, 255, 0, 255), true, 2.0f);
					}
				}
			}
			else if (m_renderMode == RenderMode::MODE_3D)
			{
				// Resize framebuffer if needed
				if ((int)viewportSize.x != m_framebuffer.getWidth() || (int)viewportSize.y != m_framebuffer.getHeight())
				{
					m_framebuffer.resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
				}

				// Setup camera and projection
				float aspect = viewportSize.x / viewportSize.y;
				float fov = 45.0f;
				float near = 0.1f;
				float far = 100.0f;
				proj = Mat4::perspective(fov, aspect, near, far);

				viewMatrix = m_camera3D->getViewMatrix();
				
				m_shader3d->enable();
				m_shader3d->setUniformMat4("pr_matrix", proj);
				m_shader3d->setUniformMat4("vw_matrix", viewMatrix);
				m_shader3d->setUniformMat4("ml_matrix", modelMatrix);

				// Draw framebuffer to ImGui
				ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
				ImVec2 imgPos = ImGui::GetItemRectMin();
				ImVec2 imgSize = ImGui::GetItemRectSize();

				ImGuizmo::SetDrawlist();
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetRect(imgPos.x, imgPos.y, imgSize.x, imgSize.y);

				if (selectedCubeIndex >= 0 && selectedCubeIndex < (int)m_scene3D.getObjects().size())
				{
					auto* cube = m_scene3D.getObjects()[selectedCubeIndex];

					Mat4 model = Mat4::translation(cube->pos) * Mat4::scale(cube->size);

					bool manipulated = ImGuizmo::Manipulate(
					viewMatrix.data(),
						proj.data(),
						ImGuizmo::TRANSLATE,
						ImGuizmo::LOCAL,
						model.data()
					);

					if (manipulated)
					{
						Vec3 newPos(model.elements[12], model.elements[13], model.elements[14]);
						cube->pos = newPos;
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();
			// File menu
			ImGui::Begin("File");
			const char* modes[] = { "2D", "3D" };
			static int currentMode = 0;

			if (ImGui::Combo("Render Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
			{
				m_renderMode = (RenderMode)currentMode;
			}
				setupFileMenu();
			ImGui::End();

			// Entities panel (Add + List)
			ImGui::Begin("Entities");
			if (m_renderMode == RenderMode::MODE_2D)
			{
				if (ImGui::Button("Add Sprite"))
				{
					auto* sp = new Sprite(0, 0, 100, 100, Vec4(1, 0, 0, 1));
					m_scene2D.addObject(sp);
				}
				bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
				int idx = 0;
				for (auto* sp : m_scene2D.getObjects())
				{
					char buf[32];
					sprintf(buf, "Sprite %d", idx);
					if (!gizmoActive && ImGui::Selectable(buf, selectedSpriteIndex == idx))
						selectedSpriteIndex = idx;
					idx++;
				}
			}
			else if (m_renderMode == RenderMode::MODE_3D)
			{
				if (ImGui::Button("Add Cube"))
				{
					auto* cube = new Cube(0, 0, 0, 1, 1, 1, Vec4(1, 0, 0, 1));
					m_scene3D.addObject(cube);
				}
				bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
				int idx = 0;
				for (auto* cube : m_scene3D.getObjects())
				{
					char buf[32];
					sprintf(buf, "Cube %d", idx);
					if (!gizmoActive && ImGui::Selectable(buf, selectedCubeIndex == idx))
						selectedCubeIndex = idx;
					idx++;
				}
			}
			ImGui::End();

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
	void Editor::setShader(Shader* shader)
	{
		shader->enable();
		shader->setUniform2f("light_pos", Vec2(4.0f, 1.5f));
		GLint texIDs[] =
		{
			0,1,2,3,4,5,6,7,8,9
		};

		shader->setUniform1iv("textures", texIDs, 10);
		float left = -m_framebuffer.getWidth() / 2.0f;
		float right = m_framebuffer.getWidth() / 2.0f;
		float bottom = -m_framebuffer.getHeight() / 2.0f;
		float top = m_framebuffer.getHeight() / 2.0f;
		float near = -1.0f;
		float far = 1.0f;

		Mat4 proj = Mat4::orthographic(left, right, bottom, top, near, far);
		shader->setUniformMat4("pr_matrix", proj);
	}

	void Editor::setShader3D(Fay::Shader* shader)
	{
		float fov = 70.0f;
		float aspect = (float)m_framebuffer.getWidth() / (float)m_framebuffer.getHeight();
		float near = 0.1f;
		float far = 100.0f;

		Mat4 model = Mat4::translation(Fay::Vec3(0, 0, 0));

		Mat4 proj = Mat4::perspective(fov, aspect, near, far);

		Mat4 view = m_camera3D->getViewMatrix();

		shader->enable();
		shader->setUniformMat4("pr_matrix", proj);
		shader->setUniformMat4("vw_matrix", view);
		shader->setUniformMat4("ml_matrix", model);
	}

	void Editor::setupDockspace()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

		ImGui::Begin("DockSpace", nullptr, flags);
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	void Editor::loadScene(const std::string& path)
	{
		if (path.ends_with(".scene_2d"))
		{
			if (m_scene2D.loadFromFile(path, m_textureManager))
			{
				std::cout << "Scene loaded: " << path << std::endl;
				m_renderLayer->clear();
				for (auto sprite : m_scene2D.getObjects())
				{
					m_renderLayer->add(sprite);
				}
			}
			else
			{
				std::cerr << "Failed to load 2D scene: " << path << std::endl;
			}
		}
		else if (path.ends_with(".scene_3d"))
		{
			if (m_scene3D.loadFromFile(path))
			{
				std::cout << "Scene loaded: " << path << std::endl;
				m_renderLayer3D->clear();

				for (auto cube : m_scene3D.getObjects())
				{
					m_renderLayer3D->add(cube);
				}
			}
			else
			{
				std::cerr << "Failed to load 3D scene" << path << std::endl;
			}
		}
		else
		{
			std::cerr << "Unkown scene type: " << path << std::endl;
		}
	}
	void Editor::saveScene(const std::string& path)
	{
		if (path.ends_with(".scene_2d"))
		{
			if (m_scene2D.saveToFile(path))
				std::cout << "Scene saved: " << path << std::endl;
			else
				std::cerr << "Failed to save 2D scene" << path << std::endl;
		}
		else if (path.ends_with(".scene_3d"))
		{
			if (m_scene3D.saveToFile(path))
				std::cout << "Scene saved: " << path << std::endl;
			else
				std::cerr << "Failed to save 2D scene" << path << std::endl;
		}
		else
		{
			std::cerr << "Failed to save, unkown scene type: " << path << std::endl;
		}
	}
	
	void Editor::setupFileMenu()
	{
		static std::string sceneDir = "Res\\Scenes\\";
		// 2d
		static std::vector<std::string> scenes2D;
		static int selectedScene2DIndex = -1;
		static bool shouldRefreshScenes = true;
		// 3d
		static std::vector<std::string> scenes3D;
		static int selectedScene3DIndex = -1;
		if (shouldRefreshScenes) {
			scenes2D = m_scene2D.listScenesInDirectory(sceneDir);
			scenes3D = m_scene3D.listScenesInDirectory(sceneDir);
			selectedScene2DIndex = -1;
			selectedScene3DIndex = -1;
			shouldRefreshScenes = false;
		}
		ImGui::Text("Select scene");

		if (ImGui::BeginListBox("Scenes"))
		{
			for (int i = 0; i < scenes2D.size(); i++)
			{
				bool isSelected2D = (selectedScene2DIndex == i);
				if (ImGui::Selectable(scenes2D[i].c_str(), isSelected2D))
					selectedScene2DIndex = i;

				if(isSelected2D)
					ImGui::SetItemDefaultFocus();
			}
			for (int j = 0; j < scenes3D.size(); j++)
			{
				bool isSelected3D = (selectedScene3DIndex == j);
				if (ImGui::Selectable(scenes3D[j].c_str(), isSelected3D))
					selectedScene3DIndex = j;

				if (isSelected3D)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndListBox();
		}
		if (selectedScene2DIndex >= 0)
		{
			std::string fullPath = sceneDir + scenes2D[selectedScene2DIndex];
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				loadScene(fullPath);  // Double-click loads the scene
			}
			if (ImGui::Button("Delete Scene"))
			{
				if (m_scene2D.deleteSceneFile(fullPath))
				{
					std::cout << "Deleted scene: " << scenes2D[selectedScene2DIndex] << std::endl;
					scenes2D = m_scene2D.listScenesInDirectory(sceneDir);
					selectedScene2DIndex = 0;
				}
				else {
					std::cout << "Failed to delete scene: " << scenes2D[selectedScene2DIndex] << std::endl;
				}
				shouldRefreshScenes = true;
			}
		}
		else if (selectedScene3DIndex >= 0)
		{
			std::string fullPath = sceneDir + scenes3D[selectedScene3DIndex];
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				loadScene(fullPath);  // Double-click loads the scene
			}
			if (ImGui::Button("Delete Scene"))
			{
				if (m_scene3D.deleteSceneFile(fullPath))
				{
					std::cout << "Deleted scene: " << scenes3D[selectedScene3DIndex] << std::endl;
					scenes3D = m_scene3D.listScenesInDirectory(sceneDir);
					selectedScene3DIndex = 0;
				}
				else {
					std::cout << "Failed to delete scene: " << scenes3D[selectedScene3DIndex] << std::endl;
				}
				shouldRefreshScenes = true;
			}
		}
		if (ImGui::Button("Save Scene"))
		{
			showSaveDialog = true;
			ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "Save Scene File", ".scene_2d, .scene_3d");
			shouldRefreshScenes = true;
		}
		if (showSaveDialog)
		{
			if (ImGuiFileDialog::Instance()->Display("SaveScene"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
					//(filePath.c_str());
					saveScene(filePath);
				}
				ImGuiFileDialog::Instance()->Close();
				showSaveDialog = false;
			}
		}
	}
}