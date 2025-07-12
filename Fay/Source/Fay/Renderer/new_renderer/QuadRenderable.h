#pragma once

#include <Renderer/Renderable2d.h>

namespace Fay
{
	class QuadRenderable : public Renderable2D
	{
	public:
		QuadRenderable(float x, float y, float width, float height, const Vec4& color);
	};
}