#pragma once

#include <FreeImage.h>
#include <string>
#include <glew.h>
#include <Utils/ImageLoad.h>

namespace Fay
{
	class Texture
	{
	private:
		std::string m_fileName;
		GLuint m_tid;
		GLsizei m_width, m_height;

	public:
		Texture(const std::string& filename);
		~Texture();
		void bind() const;
		void unbind() const;

		inline const unsigned int getId() const { return m_tid; }
		inline const unsigned int getWidth() const { return m_width; }
		inline const unsigned int getHeight() const { return m_height; }
	private:
		GLuint load();

	};
}