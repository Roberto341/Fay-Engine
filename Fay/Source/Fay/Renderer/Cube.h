#pragma once
#include <Renderer/Renderable.h>
namespace Fay
{
		class Cube : public Renderable
		{
		public: 
			uint32_t& id;
			Vec3& pos;
			Vec3& size;
			Vec4& color;
			//bool hasCollision;
		public:
			Cube(uint32_t& id, float x, float y, float z, float width, float height, float depth, const Vec4& color);
			
			void setPosition(Vec3& pos);
			Mat4 getModelMatrix() const {
				return Mat4::translation(pos);
			}
			//void setCollision(bool hasCollision);
			//bool checkCollision(bool hasCollision, Cube* obj1, Cube* obj2);
			Cube* getCube();
		};
	}