#pragma once
#include <Renderer/Renderable2d.h>

namespace Fay {

		class Sprite : public Renderable2D
		{
		public:
			Vec3& pos;
			Vec2& size;
			Vec4& color;
			bool hasCollision;
		public:
			Sprite(float x, float y, float width, float height, const Vec4& color);
			Sprite(float x, float y, float width, float height, Texture* texture);

			void setPosition(Vec3& pos);
			void setCollision(bool hasCollision);
			bool checkCollision(bool hasCollision, Sprite* obj1, Sprite* obj2);

			Sprite* getSprite();
		};
}