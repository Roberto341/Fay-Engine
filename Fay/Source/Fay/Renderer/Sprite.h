#pragma once
#include <Renderer/Renderable2d.h>

namespace Fay {

		class Sprite : public Renderable2D
		{
		public:
			Vec3& pos;
			Vec2& size;
		public:
			Sprite(float x, float y, float width, float height, const Vec4& color);
			Sprite(float x, float y, float width, float height, Texture* texture);

			void setPosition(Vec3& pos);
			bool checkCollision(float object1_x, float object1_y, float object1_width, float object1_height,
			float object2_x, float object2_y, float object2_width, float object2_height);

			Sprite* getSprite();
		};
}