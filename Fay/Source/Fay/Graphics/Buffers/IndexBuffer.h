#pragma once

#include <glew.h>

namespace Fay
{
	class IndexBuffer
	{
	private:
		GLuint m_bufferId;
		GLuint m_count;
	public:
		IndexBuffer(GLushort* data, GLsizei count);
		IndexBuffer(GLuint* data, GLsizei count);
		~IndexBuffer();
		void bind() const;
		void unbind() const;

		inline GLuint getCount() const { return m_count; }
	};
}