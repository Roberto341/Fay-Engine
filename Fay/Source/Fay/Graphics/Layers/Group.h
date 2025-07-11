#pragma once

#include <Renderer/Renderable2d.h>
namespace Fay
{
	class Group : public Renderable2D
	{
	private:
		std::vector<Renderable2D*> m_renderables;
		Mat4 m_transformationMatrix;

	public:
		Group(const Mat4& transform);
		~Group();
		void add(Renderable2D* renderable);
		void submit(Renderer2D* renderer) const override;
	};
}