#include <Renderer/Mesh.h>

namespace Fay
{
	Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
		: m_vertices(vertices), m_indicies(indices)
	{
	}
	void Mesh::setVertices(const std::vector<MeshVertex>& vertices)
	{
		m_vertices = vertices;
	}

	void Mesh::setIndicies(const std::vector<uint32_t>& indices)
	{
		m_indicies = indices;
	}

	void Mesh::setVAO(GLuint id)
	{
		m_vao = id;
	}

	const std::vector<MeshVertex>& Mesh::getVertices() const
	{
		return m_vertices;
	}

	const std::vector<uint32_t>& Mesh::getIndices() const
	{
		return m_indicies;
	}

}
