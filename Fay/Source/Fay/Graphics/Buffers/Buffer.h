#pragma once

#include <glew.h>

namespace Fay
{
	class Buffer
	{
	private:
		GLuint m_bufferId;
		GLuint m_compCount;

	public:
		Buffer(GLfloat* data, GLsizei count, GLuint compCount);
		~Buffer();
		void bind() const;
		void unbind() const;

		inline GLuint getCompCount() const { return m_compCount; }
	};
}