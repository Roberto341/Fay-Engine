#include <Renderer/MeshRenderer.h>
namespace Fay
{
	MeshRenderer::MeshRenderer()
	{

	}

	void MeshRenderer::draw(const Mesh& mesh, const Mat4& transform)
	{
		glBindVertexArray(mesh.getVAO());

		glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
	}

	void MeshRenderer::uploadMesh(Mesh& mesh)
	{
		if (mesh.getVAO() != 0)
			return;

		glGenVertexArrays(1, &mesh.m_vao);

		glGenBuffers(1, &mesh.m_vbo);

		glGenBuffers(1, &mesh.m_ebo);

		glBindVertexArray(mesh.m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vbo);

		glBufferData(GL_ARRAY_BUFFER, mesh.getVertices().size() * sizeof(MeshVertex), mesh.getVertices().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_ebo);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getIndices().size() * sizeof(uint32_t), mesh.getIndices().data(), GL_STATIC_DRAW);

		// position

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(MeshVertex),
			(void*)offsetof(MeshVertex, position)
		);

		// uv
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(MeshVertex),
			(void*)offsetof(MeshVertex, uv)
		);
		// normal 
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(MeshVertex),
			(void*)offsetof(MeshVertex, normal)
		);

		glBindVertexArray(0);
	}


}
