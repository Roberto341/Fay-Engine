#pragma once

#include <deque>
#include <Renderer/Renderer2d.h>
#include <Renderer/StaticSprite.h>

namespace Fay
{
	class SimpleRenderer : public Renderer2D
	{
	private:
		std::deque<const StaticSprite*> m_renderQueue;
	public:
		void submit(const Renderable2D* renderable) override;
		void flush() override;
	};
}