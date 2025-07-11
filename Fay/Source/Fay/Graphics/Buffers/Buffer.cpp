#include <Graphics/Buffers/Buffer.h>

namespace Fay
{
	Buffer::Buffer(GLfloat* data, GLsizei count, GLuint compCount)
		: m_compCount(compCount)
	{
		m_compCount = compCount;

		glGenBuffers(1, &m_bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &m_bufferId);
	}

	void Buffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
	}

	void Buffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
	}
}