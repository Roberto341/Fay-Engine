#pragma once

#include <vector>
#include <Graphics/Font/Font.h>

namespace Fay
{
	class FontManager
	{
	public:
		FontManager();
		~FontManager();
		static void add(Font* font);
		static Font* getFont(const std::string& name);
		static Font* getFont(const std::string& name, unsigned int size);
		static void clean();
	private:
		static std::vector<Font*> m_fonts;
	};
}