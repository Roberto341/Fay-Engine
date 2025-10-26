#pragma once
#include <Renderer/Renderable.h>
#include <Entity/Entity.h>
namespace Fay {
		class Sprite : public Renderable
		{
		public:
			Vec3& pos;
			Vec3& size;
			Vec4& color;
			bool hasCollision;
		public:
			Sprite(float x, float y, float z, float width, float height, float depth, const Vec4& color);
			Sprite(float x, float y, float z, float width, float height, float depth, Texture* texture);

			void setPosition(Vec3& pos);
			void setCollision(bool hasCollision);
			bool checkCollision(bool hasCollision, Sprite* obj1, Sprite* obj2);
			Sprite* getSprite();
		};
}