#include <Graphics/Font/Font.h>

namespace Fay
{
	Font::Font(std::string name, std::string filename, int size)
		: m_name(name), m_filename(filename), m_size(size)
	{
		m_ftAtlas = ftgl::texture_atlas_new(512, 512, 2);
		m_ftFont = ftgl::texture_font_new_from_file(m_ftAtlas, size, filename.c_str());
	}

	Font::~Font()
	{

	}
}