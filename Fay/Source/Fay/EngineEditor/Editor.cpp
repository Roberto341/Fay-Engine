#include <EngineEditor/Editor.h>
#include <cmath>
namespace Fay
{
	EntityID Editor::s_SelectedEntity = 0;
	SceneType Editor::s_ActiveScene = SceneType::None;
	bool Editor::shouldRefreshScenes = false;
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
		
		setShader(m_shader);
		setShader3D(m_shader3d);

		m_batchRenderer = new BatchRenderer();
		m_renderLayer = new TileLayer(m_batchRenderer);
		m_renderLayer->setShader(m_shader);
		m_Scene.setSceneType(SceneType::Scene2D);
		m_lastTime = glfwGetTime();
		selectedEntityID = -1;
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
			//auto& objects = m_scene2D.getObjects();

			// new render mode
			if (!m_Scene.getObjects().empty())
			{
				m_renderLayer->clear();
				for (auto* sp : m_Scene.getObjects())
				{
					m_renderLayer->add(sp);
				}
				m_renderLayer->render();
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
							TransformComponent transform(sprite->getPosition(), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(sprite->getSize().x, sprite->getSize().y, 0));
							ComponentManager<TransformComponent>::Get().addComponent(entity, transform);
						}
					}

					if (ComponentManager<CollisionSpriteComponent>::Get().hasComponent(entity))
					{
						if (ImGui::Button("Remove Collision"))
						{
							ComponentManager<CollisionSpriteComponent>::Get().removeComponent(entity);
						}
					}
					else
					{
						if (ImGui::Button("Add Collision"))
						{
							CollisionSpriteComponent hitBox(sprite->getPosition(), sprite->getSize());
							ComponentManager<CollisionSpriteComponent>::Get().addComponent(entity, hitBox);
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

					ImGui::InputText("Sprite ID", idBuf, IM_ARRAYSIZE(idBuf), ImGuiInputTextFlags_ReadOnly);
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
						comp->color = cube->getColor();
					float uniformScale = cube->getSize().x;
					if (ImGui::SliderFloat("Scale", &uniformScale, 0.1f, 500.0f))
					{
						cube->setSize(Vec3(uniformScale, uniformScale, uniformScale));
					}
					if (comp)
						comp->size = cube->getSize();
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
						comp->position = cube->getPosition();
				}
			}
			ImGui::End();
			// Viewport window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			ImVec2 viewportPos = ImGui::GetWindowPos();
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImVec2 mouse = ImGui::GetMousePos();
			ImVec2 viewportPosImGui = ImGui::GetItemRectMin(); 
			ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
			ImVec2 imgPos = ImGui::GetItemRectMin();
			ImVec2 imgSize = ImGui::GetItemRectSize();

			Vec2 viewPos(viewportPosImGui.x, viewportPosImGui.y);

			// Final result
			Vec2 relativeMousePos = {
				mouse.x - cursorPos.x,
				(imgPos.y + viewportSize.y) - mouse.y
			};
			//relativeMousePos.y = viewportPos.y - relativeMousePos.y;
			Vec2 worldMousePos = {
				relativeMousePos.x - (viewportSize.x / 2.0f),
				relativeMousePos.y - (viewportSize.y / 2.0f)
			};


			bool hoveredViewport = ImGui::IsItemHovered();
			bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
			bool clickedLeft = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			if (hoveredViewport && clickedLeft && !gizmoActive)
			{
				selectedEntityID = -1;
			}
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
				}
				ImGui::InvisibleButton("viewport_btn", viewportSize);

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
				if (selectedEntityID != -1)
				{
					EntityID entity = selectedEntityID;
					auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(entity);
					if (!comp) return;

					auto* sprite = comp->sprite;

					float left = -m_framebuffer.getWidth() / 2.0f;
					float right = m_framebuffer.getWidth() / 2.0f;
					float bottom = -m_framebuffer.getHeight() / 2.0f;
					float top = m_framebuffer.getHeight() / 2.0f;
					float near = -1.0f;
					float far = 1.0f;

					Mat4 model = Mat4::translation(sprite->getPosition()) *
						Mat4::scale(Vec3(sprite->getSize().x, sprite->getSize().y, 1.0f));

					viewMatrix = Mat4::identity(); //  not using camera
					proj = Mat4::orthographic(-imgSize.x / 2, imgSize.x / 2,
						-imgSize.y / 2, imgSize.y / 2,
						-1, 1);

					bool manipulated = ImGuizmo::Manipulate(
						viewMatrix.data(),
						proj.data(),
						ImGuizmo::TRANSLATE,
						ImGuizmo::LOCAL,
						model.data()
					);

					if (manipulated) {
						// Update your sprite position
						Vec3 newPos(model.elements[12], model.elements[13], model.elements[14]);
						sprite->setPosition(newPos);
					}

					Vec3 origin = sprite->getPosition();
					Vec3 size = sprite->getSize();
					Vec3 cornersWorld[4] = { 
						origin,                                         // bottom-left
						origin + Vec3(size.x, 0, 0),                    // bottom-right
						origin + Vec3(size.x, size.y, 0),               // top-right
						origin + Vec3(0, size.y, 0),                    // top-left
					};

					Mat4 view = viewMatrix;
					//Mat4 proj = camera->getProjection();

					ImVec2 screenCorners[4];
					bool valid = true;

					const float fbWidth = imgSize.x;
					const float fbHeight = imgSize.y;
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

						// Flip Y to match ImGui::Image(0,1)-(1,0)
						float screenX = imgPos.x + ndcX * fbWidth;
						float screenY = imgPos.y + (1.0f - ndcY) * fbHeight;

						screenCorners[i] = ImVec2(screenX, screenY);
					}

						auto* dl = ImGui::GetWindowDrawList();
					if (valid)
					{
						for (auto& sc : screenCorners) {
							if (ComponentManager<CollisionSpriteComponent>::Get().getComponent(entity))
							{
								dl->AddCircleFilled(sc, 3.0f, IM_COL32(0, 255, 0, 255));
							}
						}
						dl->AddPolyline(screenCorners, 4, IM_COL32(255, 255, 0, 155), true, 2.0f);
					}

					/*if (ComponentManager<TransformComponent>::Get().hasComponent(entity))
					{
						dl->AddPolyline(screenCorners, 4, IM_COL32(0, 0, 255, 255), true, 2.0f);
					}*/
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
				float fov = 70.0f;
				float near = 0.1f;
				float far = 1000.0f;
				Mat4 pj = Mat4::perspective(fov, aspect, near, far);
				m_camera3D->setPerspective(fov, aspect, near, far);
				Mat4 vm = m_camera3D->getViewMatrix();
				m_shader3d->enable();
				m_shader3d->setUniformMat4("pr_matrix", pj);
				m_shader3d->setUniformMat4("vw_matrix", vm);
				m_shader3d->setUniformMat4("ml_matrix", modelMatrix);

				// Draw framebuffer
				ImGui::Image((void*)(intptr_t)m_framebuffer.getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
				ImVec2 imgPos = ImGui::GetItemRectMin();
				ImVec2 imgSize = ImGui::GetItemRectSize();
				ImGui::InvisibleButton("viewport_btn", viewportSize, ImGuiButtonFlags_MouseButtonLeft);

				if (selectedEntityID != -1)
				{
					EntityID entity = selectedEntityID;
					auto* comp = ComponentManager<CubeComponent>::Get().getComponent(entity);
					if (!comp) return;

					auto* cube = comp->cube;
					Mat4 model = Mat4::translation(cube->getPosition()) * Mat4::scale(cube->getSize());
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetOrthographic(false);
					
					bool manip = ImGuizmo::Manipulate(
						vm.data(),
						pj.data(),
						ImGuizmo::TRANSLATE,
						ImGuizmo::WORLD,
						model.data()
					);

					if (manip)
					{
						Vec3 newPos(model.elements[12], model.elements[13], model.elements[14]);
						cube->setPosition(newPos);
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
					m_renderLayer->setShader(m_shader3d);
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
				setupFileMenu();
			ImGui::End();
			// Entities panel (Add + List)
			ImGui::Begin("Entities");
			if (m_renderMode == RenderMode::MODE_2D)
			{
				if (ImGui::Button("Add Sprite"))
				{
					EntityID entity = m_Scene.generateEntityID();
					auto sprite = new Sprite(0, 0, 0, 100, 100, 0, Vec4(1, 0, 0, 1));

					// Generate EntityID 
					ComponentManager<SpriteComponent>::Get().addComponent(entity, SpriteComponent(sprite));
					m_Scene.addObject(sprite);
				}
				if (ImGui::Button("Delete Sprite"))
				{
					EntityID entity = static_cast<EntityID>(selectedEntityID);
					SpriteComponent* spriteComp = ComponentManager<SpriteComponent>::Get().getComponent(entity);
					if (spriteComp && spriteComp->sprite)
					{
						FAY_LOG_DEBUG("Deleting sprite entity: " << entity);

						// remove from scene 
						selectedEntityID = -1;
						m_renderLayer->remove(spriteComp->sprite);
						m_Scene.removeObject(spriteComp->sprite);
						// remove component from ECS
						ComponentManager<SpriteComponent>::Get().removeComponent(entity);

					}
				}
				auto& spriteEntities = ComponentManager<SpriteComponent>::Get().getEntities();
				int idx = 0;
				for (auto* sp : m_Scene.getObjects())
				{
					Vec3 spritePos = sp->getPosition();
					Vec3 spriteSize = sp->getSize();

					if (worldMousePos.x >= spritePos.x && worldMousePos.x <= spritePos.x + spriteSize.x &&
						worldMousePos.y >= spritePos.y && worldMousePos.y <= spritePos.y + spriteSize.y)
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							EntityID entity = spriteEntities[idx];  // <- store real entity ID
							selectedEntityID = entity;              // <- store this instead of index
							std::cout << "Selected EntityID: " << entity << std::endl;
							break;
						}
					}
					idx++;
				}
			}
			else if (m_renderMode == RenderMode::MODE_3D)
			{
				if (ImGui::Button("Add Cube"))
				{
					EntityID entity = m_Scene.generateEntityID();

					auto* cube = new Cube(0, 0, 0, 1, 1, 1, Vec4(1, 0, 0, 1));

					ComponentManager<CubeComponent>::Get().addComponent(entity, CubeComponent(cube));
					m_Scene.addObject(cube);
				}
				if (ImGui::Button("Delete Cube"))
				{
					CubeComponent* cubeComp = ComponentManager<CubeComponent>::Get().getComponent(selectedEntityID);
					if (cubeComp && cubeComp->cube)
					{
						FAY_LOG_DEBUG("Deleting cube entity: " << selectedEntityID);

						// remove from scene 
						m_Scene.removeObject(cubeComp->cube);
						m_renderLayer->remove(cubeComp->cube);
						// remove component from ECS
						ComponentManager<CubeComponent>::Get().removeComponent(selectedEntityID);
						selectedEntityID = -1;
					}
				}
				// --- Get camera matrices ---
				Mat4 proj = m_camera3D->getProjectionMatrix((float)m_framebuffer.getWidth() / (float)m_framebuffer.getHeight());
				Mat4 view = m_camera3D->getViewMatrix();

				// --- Get ray from mouse ---
				Ray ray = getRayFromMouse(mouse, viewportPos, viewportSize, proj, view);
				Vec3 rayOrigin = ray.origin;
				Vec3 rayDir = ray.dir;
				// 6. Raycast all cubes
				int selectedIdx = -1;
				float closestT = FLT_MAX;
				for (int i = 0; i < m_Scene.getObjects().size(); ++i)
				{
					auto* cube = m_Scene.getObjects()[i];
					Vec3 aabbMin = cube->getPosition() - cube->getSize() * 0.5f;
					Vec3 aabbMax = cube->getPosition() + cube->getSize() * 0.5f;

					float t;
					if (intersectRayAABB(rayOrigin, rayDir, aabbMin, aabbMax, t))
					{
						if (t > 0 && t < closestT)
						{
							closestT = t;
							selectedIdx = i;
						}
					}
				}
				bool mouseInViewport =
					mouse.x >= viewportPos.x && mouse.x <= viewportPos.x + viewportSize.x &&
					mouse.y >= viewportPos.y && mouse.y <= viewportPos.y + viewportSize.y;

				// 7. Update selection
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
								FAY_LOG_DEBUG("Selected cube entity: " << selectedEntityID);
								break;
							}
						}
					}
					else
					{
						selectedEntityID = static_cast<EntityID>(-1);
					}
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

	void Editor::createNewScene(const std::string& path)
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
		static std::string sceneDir = "Res\\Scenes\\";
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

				createNewScene(fullPath);
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
}