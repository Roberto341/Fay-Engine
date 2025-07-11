#include <Renderer/Sprite.h>

namespace Fay
{
	Sprite::Sprite(float x, float y, float width, float height, const Vec4& color)
		: Renderable2D(Vec3(x, y, 0), Vec2(width, height), color), pos(m_position), size(m_size)
	{
		m_position = Vec3(x, y, 0.0f);
		m_size = Vec2(width, height);
	}

	Sprite::Sprite(float x, float y, float width, float height, Texture* texture)
		: Renderable2D(Vec3(x, y, 0), Vec2(width, height), Vec4(1, 1, 1, 1)), pos(m_position), size(m_size)
	{
		m_position = Vec3(x, y, 0.0f);
		m_size = Vec2(width, height);
		m_texture = texture;
	}
	bool Sprite::checkCollision(float object1_x, float object1_y, float object1_width, float object1_height,
		float object2_x, float object2_y, float object2_width, float object2_height) {
		return object1_x < object2_x + object2_width &&
			object2_x < object1_x + object1_width &&
			object1_y < object2_y + object2_height &&
			object2_y < object1_y + object1_height;
	}
	Sprite* Sprite::getSprite()
	{
		return this;
	}
}