#pragma once
#include <EngineEditor/EditorCore.h>
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
	private:
		// Variables
		std::unique_ptr<FrameBuffer> m_framebuffer;
		EditorUtils* m_utils = nullptr;
		int m_width = 0;
		int m_height = 0;
	};
}