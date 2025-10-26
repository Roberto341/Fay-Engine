#pragma once

#include <Renderer/Renderable.h>
namespace Fay
{
	class Label : public Renderable
	{
	public:
		Font* m_font;
		std::string text;
		Vec3& position;
		Vec4& color;
		float x, y;
	public:
		Label(std::string text, float x, float y, Font* font, Vec4 color);
		void submit(Renderer* renderer) const override;
	};
}