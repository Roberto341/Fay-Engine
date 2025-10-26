#include <Graphics/Label.h>

namespace Fay
{
	Label::Label(std::string text, float x, float y, Font* font, Vec4 color)
		: Renderable(), text(text), position(m_position), color(m_color), m_font(font)
	{
		m_position = Vec3(x, y, 0.0f);
		m_color = color;
	}
	void Label::submit(Renderer* renderer) const
	{
		renderer->drawString(text, position, *m_font, m_color);
	}
}