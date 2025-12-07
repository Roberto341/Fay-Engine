#pragma once
#include <Renderer/Renderable.h>
#include <Entity/Entity.h>
namespace Fay {
		class Sprite : public Renderable
		{
		public:
			uint32_t& id;
			Vec3& pos;
			Vec3& size;
			Vec4& color;
			bool hasCollision;
		public:
			Sprite(uint32_t& id, float x, float y, float z, float width, float height, float depth, const Vec4& color);
			Sprite(uint32_t& id, float x, float y, float z, float width, float height, float depth, Texture* texture);

		};
}