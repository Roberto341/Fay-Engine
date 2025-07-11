#include <Graphics/Buffers/VertexArray.h>

namespace Fay
{
	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_arrayId);
	}

	VertexArray::~VertexArray()
	{
		for (int i = 0; i < m_buffers.size(); i++)
		{
			delete m_buffers[i];
		}
		glDeleteVertexArrays(1, &m_arrayId);
	}

	void VertexArray::addBuffer(Buffer* buffer, GLuint index)
	{
		bind();
		buffer->bind();

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, buffer->getCompCount(), GL_FLOAT, GL_FALSE, 0, 0);

		buffer->unbind();
		unbind();
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(m_arrayId);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}
}