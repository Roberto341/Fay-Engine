#include <Renderer/3DRenderer/Cube.h>
namespace Fay
{
		Cube::Cube(float x, float y, float z, float width, float height, float depth, const Vec4& color)
			: Renderable3D(Vec3(x, y, z), Vec3(width, height, depth), color), pos(m_position), size(m_size), color(m_color)
		{
			m_position = Vec3(x, y, z);
			m_size = Vec3(width, height, depth);
		}
		void Cube::setPosition(Vec3& pos)
		{
			m_position = Vec3(pos.x, pos.y, pos.z);
		}
		Cube* Cube::getCube()
		{
			return this;
		}
}