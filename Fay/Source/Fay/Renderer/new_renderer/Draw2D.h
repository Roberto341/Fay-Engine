#pragma once

#include <Renderer/Renderer2d.h>
#include <Renderer/Renderable2d.h>
#include <Renderer/Sprite.h>
namespace Fay
{
	class Draw2D
	{
	public:
		static void setRenderer(Renderer2D* renderer);
		static void drawQuad(float x, float y, float width, float height, const Vec4& color);
	private:
		static Renderer2D* s_renderer;
	};
}