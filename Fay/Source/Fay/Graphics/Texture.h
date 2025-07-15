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
		std::string m_name;
		std::string m_fileName;
		GLuint m_tid;
		GLsizei m_width, m_height;

	public:
		Texture(std::string name, std::string filename);
		~Texture();
		void bind() const;
		void unbind() const;

		inline const unsigned int getId() const { return m_tid; }
		inline const unsigned int getWidth() const { return m_width; }
		inline const unsigned int getHeight() const { return m_height; }
		inline const std::string& getName()const { return m_name; }
		inline const std::string& getFilename()const { return m_fileName; }
	private:
		GLuint load();

	};
}