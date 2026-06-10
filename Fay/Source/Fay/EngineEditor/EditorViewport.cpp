#include <EngineEditor/EditorViewport.h>

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
		ImVec2 mouse = ImGui::GetMousePos();
		//ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		Resize((int)viewportSize.x, (int)viewportSize.y);

		// --- Draw framebuffer ---
		ImGui::Image((void*)(intptr_t)m_framebuffer->getTexture(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImVec2 imgPos = ImGui::GetItemRectMin();
		ImVec2 imgSize = ImGui::GetItemRectSize();

		bool hoveredViewport = ImGui::IsItemHovered();
		bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		bool gizmoActive = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
		
		// --- ViewportContext Build context ---
		ViewportContext ctx;
		ctx.mousePos = mouse;
		ctx.viewportPos = viewportPos;
		ctx.viewportSize = viewportSize;
		ctx.imgPos = imgPos;
		ctx.imgSize = imgSize;
		//ctx.clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		if (hoveredViewport && clicked && !gizmoActive)
		{
			ctx.clicked = true;
		}
		else {
			ctx.clicked = false;
		}
		// --- Run active mode ---
		if (m_viewportMode)
		{
			m_viewportMode->Setup(m_utils, viewportSize, ctx);
			m_viewportMode->HandleGizmos(m_utils, ctx);
			m_viewportMode->HandleSelection(m_utils, ctx);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorViewport::Shutdown()
	{
		m_framebuffer.reset();
	}
}