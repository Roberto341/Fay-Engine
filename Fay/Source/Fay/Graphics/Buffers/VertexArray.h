#pragma once

#include <vector>
#include <glew.h>
#include <Graphics/Buffers/Buffer.h>

namespace Fay
{
	class VertexArray
	{
	private:
		GLuint m_arrayId;
		std::vector<Buffer*> m_buffers;
	public:
		VertexArray();
		~VertexArray();

		void addBuffer(Buffer* buffer, GLuint index);
		void bind() const;
		void unbind() const;
	};
}