#pragma once

#include <Renderer/Renderable.h>
#include <Renderer/Mesh.h>
namespace Fay
{
	class MeshObject : public Renderable
	{
	private:
		Mesh* m_mesh;

	public:
		MeshObject(uint32_t& id, Mesh* mesh, float x, float y, float z, float width, float height, float depth, const Vec4& color);

		Mesh* getMesh();
		const Mesh* getMesh() const;

		void setMesh(Mesh* mesh);
	};
}