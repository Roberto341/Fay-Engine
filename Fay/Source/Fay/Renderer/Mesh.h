#pragma once

#include <vector>
#include <glew.h>
#include <Math/Vec2.h>
#include <Math/Vec3.h>

namespace Fay
{
	struct MeshVertex
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};
	class Mesh
	{
	friend class MeshRenderer;
	private:
		std::vector<MeshVertex> m_vertices;
		std::vector<uint32_t> m_indicies;
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
		GLuint m_ebo = 0;

	public:
		Mesh() = default;

		Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);
		
		void setVertices(const std::vector<MeshVertex>& vertices);
		void setIndicies(const std::vector<uint32_t>& indices);
		void setVAO(GLuint id);
		GLuint getVAO() const { return m_vao; }

		const std::vector<MeshVertex>& getVertices() const;
		
		const std::vector<uint32_t>& getIndices() const;
		
	};
}