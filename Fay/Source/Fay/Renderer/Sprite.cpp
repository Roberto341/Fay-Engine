#include <Renderer/Sprite.h>

namespace Fay
{
	Sprite::Sprite(float x, float y, float z, float width, float height, float depth, const Vec4& color)
		: Renderable(Vec3(x, y, z), Vec3(width, height, 0), color), pos(m_position), size(m_size), hasCollision(false), color(m_color)
	{
		m_position = Vec3(x, y, z);
		m_size = Vec3(width, height, depth);
		m_collision = false;
	}
	Sprite::Sprite(float x, float y, float z, float width, float height, float depth, Texture* texture)
		: Renderable(Vec3(x, y, z), Vec3(width, height, 0), Vec4(1, 1, 1, 1)), pos(m_position), size(m_size), hasCollision(false), color(m_color)
	{
		m_position = Vec3(x, y, z);
		m_size = Vec3(width, height, depth);
		m_texture = texture;
		m_collision = false;

	}
	void Sprite::setPosition(Vec3& pos)
	{
		m_position = Vec3(pos.x, pos.y, pos.z);
	}
	void Sprite::setCollision(bool hasCollision)
	{
		m_collision = hasCollision;
	}
	bool Sprite::checkCollision(bool hasCollision, Sprite* obj1, Sprite* obj2)
	{
		if (hasCollision)
		{
			return obj1->pos.x < obj2->pos.x + obj2->size.x &&
				obj2->pos.x < obj1->pos.x + obj1->size.x &&
				obj1->pos.y < obj2->pos.y + obj2->size.y &&
				obj2->pos.y < obj1->pos.y + obj1->size.y;
		}
	}

	Sprite* Sprite::getSprite()
	{
		return this;
	}
}