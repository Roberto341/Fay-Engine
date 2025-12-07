#include <Renderer/Sprite.h>

namespace Fay
{
	Sprite::Sprite(uint32_t& id, float x, float y, float z, float width, float height, float depth, const Vec4& color)
		: Renderable(id, Vec3(x, y, z), Vec3(width, height, 0), color), pos(m_position), size(m_size), hasCollision(false), color(m_color), id(m_id)
	{
		m_id = id;
		m_position = Vec3(x, y, z);
		m_size = Vec3(width, height, depth);
		m_collision = false;
	}
	Sprite::Sprite(uint32_t& id, float x, float y, float z, float width, float height, float depth, Texture* texture)
		: Renderable(id, Vec3(x, y, z), Vec3(width, height, 0), Vec4(1, 1, 1, 1)), pos(m_position), size(m_size), hasCollision(false), color(m_color), id(m_id)
	{
		m_id = id;
		m_position = Vec3(x, y, z);
		m_size = Vec3(width, height, depth);
		m_texture = texture;
		m_collision = false;
	}
}