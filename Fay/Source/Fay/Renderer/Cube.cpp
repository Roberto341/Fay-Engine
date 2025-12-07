#include <Renderer/Cube.h>
namespace Fay
{
		//RenderDimension dimension = RenderDimension::D2
		Cube::Cube(uint32_t& id, float x, float y, float z, float width, float height, float depth, const Vec4& color)
			: Renderable(id, Vec3(x, y, z), Vec3(width, height, depth), color, RenderDimension::D3), pos(m_position), size(m_size), color(m_color), id(m_id)
		{
			m_id = id;
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