#include <Renderer/new_renderer/QuadRenderable.h>

namespace Fay
{
	QuadRenderable::QuadRenderable(float x, float y, float width, float height, const Vec4& color)
	{
		m_position = Vec3(x, y, 0.0f);
		m_size = Vec2(width, height);
		m_color = color;
		m_uv = Renderable2D::getUV();
		m_texture = 0;
	}

}
