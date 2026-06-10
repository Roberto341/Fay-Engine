#pragma once
#include <EngineEditor/EditorCore.h>
#include <ImGui/imgui.h>
#include <EngineEditor/Utils/ViewportMode2D.h>
#include <EngineEditor/Utils/ViewportMode3D.h>
#include <Graphics/Buffers/FrameBuffer.h>
#include <Math/Mat4.h>
#include <Math/Vec2.h>
#include <Math/Vec3.h>
namespace Fay
{
	class EditorViewport
	{
	private:
		// Methods
	public:
		EditorViewport();
		~EditorViewport();

		void Init(int width = 1280, int height = 720);

		void Resize(int viewWidth, int viewHeight);

		void RenderBegin();
		void RenderEnd();

		void DrawViewport();

		void SetUtils(EditorUtils* util) { m_utils = util; }
		void Shutdown();

		void SetViewportMode(std::unique_ptr<ViewportMode> mode) { m_viewportMode = std::move(mode); }
	private:
		// Variables
		std::unique_ptr<FrameBuffer> m_framebuffer;
		EditorUtils* m_utils = nullptr;
		int m_width = 0;
		int m_height = 0;
		std::unique_ptr<ViewportMode> m_viewportMode;
	};
}