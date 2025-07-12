#include <Renderer/new_renderer/Draw2D.h>

namespace Fay
{
	Renderer2D* Draw2D::s_renderer = nullptr;

	void Draw2D::setRenderer(Renderer2D* renderer)
	{
		s_renderer = renderer;
	}
	void Draw2D::drawQuad(float x, float y, float width, float height, const Vec4& color)
	{
		if (!s_renderer) return;

		Sprite* quad = new Sprite(x, y, width, height, color);
		s_renderer->submit(quad);
	}
}