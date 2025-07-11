#pragma once
#include <string>
#include "../../../../Ext/Freetype-gl/freetype-gl.h"
namespace Fay
{
	class Font
	{
	public:
		Font(std::string name, std::string filename, int size);
		inline ftgl::texture_font_t* getFTGLFont() const { return m_ftFont; }

		inline const unsigned int getID() const { return m_ftAtlas->id; }
		inline const std::string& getName()const { return m_name; }
		inline const std::string& getFilename()const { return m_filename; }
		inline const int getSize() const { return m_size; }
		~Font();
	private:
		ftgl::texture_atlas_t* m_ftAtlas;
		ftgl::texture_font_t* m_ftFont;
		unsigned int m_size;

		std::string m_name;
		std::string m_filename;
	};
}