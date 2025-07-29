#pragma once
#include <Renderer/3DRenderer/Renderable3d.h>

namespace Fay
{
		class Cube : public Renderable3D
		{
		public: 
			Vec3& pos;
			Vec3& size;
			Vec4& color;
		public:
			Cube(float x, float y, float z, float width, float height, float depth, const Vec4& color);
			
			void setPosition(Vec3& pos);
			Mat4 getModelMatrix() const {
				return Mat4::translation(pos);
			}
			Cube* getCube();
		};
	}