#include <Renderer/MeshObject.h>

namespace Fay
{
	MeshObject::MeshObject(uint32_t& id, Mesh* mesh, float x, float y, float z, float width, float height, float depth, const Vec4& color)
		: Renderable(id, Vec3(x, y, z), Vec3(width, height, depth), color, RenderDimension::D3), m_mesh(mesh)
	{
		m_id = id;
		m_position = Vec3(x, y, z);
		m_size = Vec3(width, height, depth);
	}
	Mesh* MeshObject::getMesh()
	{
		return m_mesh;
	}
	const Mesh* MeshObject::getMesh() const
	{
		return m_mesh;
	}
	void MeshObject::setMesh(Mesh* mesh)
	{
		m_mesh = mesh;
	}
}