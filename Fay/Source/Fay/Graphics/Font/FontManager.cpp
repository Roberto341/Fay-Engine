#include <Graphics/Font/FontManager.h>

namespace Fay
{
	std::vector<Font*> FontManager::m_fonts;

	void FontManager::add(Font* font)
	{
		m_fonts.push_back(font);
	}

	void FontManager::clean()
	{
		for (int i = 0; i < m_fonts.size(); i++)
		{
			delete m_fonts[i];
		}
	}

	Font* FontManager::getFont(const std::string& name)
	{
		for (Font* font : m_fonts)
		{
			if (font->getName() == name)
			{
				return font;
			}
		}
		return nullptr;
	}
	Font* FontManager::getFont(const std::string& name, unsigned int size)
	{
		for (Font* font : m_fonts)
		{
			if (font->getSize() == size && font->getName() == name)
			{
				return font;
			}
		}
		return nullptr;
	}

	FontManager::FontManager()
	{

	}

	FontManager::~FontManager()
	{

	}
}