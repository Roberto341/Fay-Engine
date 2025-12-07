#include <EngineEditor/EditorViewport.h>
#include "EditorCore.h"

namespace Fay
{
	EditorViewport::EditorViewport() = default;
	EditorViewport::~EditorViewport() = default;	

	void EditorViewport::Init(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_framebuffer = std::make_unique<FrameBuffer>((uint32_t)width, (uint32_t)height);
	}

	void EditorViewport::Resize(int viewWidth, int viewHeight)
	{
		if (viewWidth == m_width && viewHeight == m_height) return;
		m_width = viewWidth;
		m_height = viewHeight;
		if(!m_framebuffer)
			m_framebuffer = std::make_unique<FrameBuffer>((uint32_t)m_width, (uint32_t)m_height);
		else
			m_framebuffer->resize((uint32_t)m_width, (uint32_t)m_height);
	}

	void EditorViewport::RenderBegin()
	{
		if (m_framebuffer)
		{
			m_framebuffer->bind();
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void EditorViewport::RenderEnd()
	{
		if (m_framebuffer)
		{
			m_framebuffer->unbind();
		}
	}

	void EditorViewport::DrawViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		// --- Get viewport and mouse info --- 
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 viewportPos = ImGui::GetWindowPos();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 mouse = ImGui::GetMousePos();

		// --- Draw framebuffer to viewport ---
		ImGui::Image((void*)(intptr_t)m_framebuffer->getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImVec2 imgPos = ImGui::GetItemRectMin();
		ImVec2 imgSize = ImGui::GetItemRectSize();

		// --- Convert mouse to viewport-local coordinates ---
		Vec2 relativeMousePos =
		{
			mouse.x - imgPos.x,
			(imgPos.y + viewportSize.y) - mouse.y
		};
		Vec2 worldMousePos =
		{
			relativeMousePos.x - (viewportSize.x / 2.0f),
			relativeMousePos.y - (viewportSize.y / 2.0f)
		};

		bool hoveredViewport = ImGui::IsItemHovered();
		bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
		bool clickedLeft = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		if (hoveredViewport && clickedLeft && !gizmoActive)
			m_utils->SetSelectedEntity(-1);

		Mat4 modelMatrix = Mat4::identity();
		Mat4 viewMatrix = Mat4::identity();
		Mat4 projMatrix = Mat4::identity();

		// --- 2D Mode ---
		if(m_utils->GetRenderMode() == RenderMode::MODE_2D)
		{
			Resize((int)viewportSize.x, (int)viewportSize.y);	
			// Ensure framebuffer size matches UI (EditorViewport already resizes)
			float halfW = viewportSize.x * 0.5f;
			float halfH = viewportSize.y * 0.5f;
			projMatrix = Mat4::orthographic(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
			viewMatrix = Mat4::identity();

			m_utils->GetShader()->enable();
			m_utils->GetShader()->setUniformMat4("pr_matrix", projMatrix);
			m_utils->GetShader()->setUniformMat4("vw_matrix", viewMatrix);

			ImGui::InvisibleButton("viewport_btn", viewportSize);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetRect(imgPos.x, imgPos.y, imgSize.x, imgSize.y);

			// --- Selected sprite gizmo ---
			if (m_utils->GetSelectedEntity() != -1)
			{
				auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(m_utils->GetSelectedEntity());
				if (comp && comp->sprite)
				{
					auto* sprite = comp->sprite;

					// --- Get sprite transform ---
					Vec3 pos = sprite->getPosition();
					Vec3 size = sprite->getSize();
					Mat4 model = Mat4::translation(pos) * Mat4::scale(Vec3(size.x, size.y, 1.0f));
					m_utils->GetShader()->setUniformMat4("ml_matrix", model);

					bool manipulated = ImGuizmo::Manipulate(viewMatrix.data(), projMatrix.data(), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, model.data());

					if (manipulated)
					{
						Vec3 newPos = Vec3(model.elements[12], model.elements[13], model.elements[14]);
						sprite->setPosition(newPos);
					}
				}
			}
			// --- Sprite Selection ---
			for (auto& obj : m_utils->GetScene()->getObjects())
			{
				Vec3 pos = obj->getPosition();
				Vec3 size = obj->getSize();

				if (worldMousePos.x >= pos.x && worldMousePos.x <= pos.x + size.x &&
					worldMousePos.y >= pos.y && worldMousePos.y <= pos.y + size.y &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					m_utils->SetSelectedEntity(obj->getId());
					break;
				}
			}
		}

		// --- 3D Mode ---
		else if (m_utils->GetRenderMode() == RenderMode::MODE_3D)
		{
			Resize((int)viewportSize.x, (int)viewportSize.y);

			float aspect = viewportSize.x / viewportSize.y;
			float fov = 70.0f;
			float near = 0.1f;
			float far = 1000.0f;

			Mat4 proj = Mat4::perspective(fov, aspect, near, far);
			Mat4 view = m_utils->GetCamera()->getViewMatrix();

			m_utils->GetShader()->enable();
			m_utils->GetShader()->setUniformMat4("pr_matrix", proj);
			m_utils->GetShader()->setUniformMat4("vw_matrix", view);
			m_utils->GetShader()->setUniformMat4("ml_matrix", modelMatrix);

			ImGui::Image((void*)(intptr_t)m_framebuffer->getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
			ImVec2 imgPos = ImGui::GetItemRectMin();
			ImVec2 imgSize = ImGui::GetItemRectSize();
			ImGui::InvisibleButton("viewport_btn", viewportSize, ImGuiButtonFlags_MouseButtonLeft);

			// --- Selected cube gizmo ---
			if (m_utils->GetSelectedEntity() != -1)
			{
				auto* comp = ComponentManager<CubeComponent>::Get().getComponent(m_utils->GetSelectedEntity());

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
			// --- Raycast selection ---
			Vec2 mousePos = Vec2(mouse.x, mouse.y);
			Vec2 vpPos = Vec2(viewportPos.x, viewportPos.y);
			Vec2 vpSize = Vec2(viewportSize.x, viewportSize.y);

			Ray ray = getRayFromMouse(mousePos, vpPos, vpSize, proj, view);
			float closestT = FLT_MAX;
			int selectedIdx = -1;
			for (int i = 0; i < m_utils->GetScene()->getObjects().size(); i++)
			{
				auto* cube = m_utils->GetScene()->getObjects()[i];
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
			
			if(mouseInViewport && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				if (selectedIdx != -1)
				{
					auto* selectedObj = m_utils->GetScene()->getObjects()[selectedIdx];
					auto& cubeEntities = ComponentManager<CubeComponent>::Get().getEntities();
					for (EntityID e : cubeEntities)
					{
						CubeComponent* comp = ComponentManager<CubeComponent>::Get().getComponent(e);
						if(comp && comp->cube == selectedObj)
						{
							m_utils->SetSelectedEntity(e);
							break;
						}
					}
				}
				else
				{
					m_utils->SetSelectedEntity(-1);
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorViewport::Shutdown()
	{
		m_framebuffer.reset();
	}
}