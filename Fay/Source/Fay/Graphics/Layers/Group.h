#pragma once

#include <Renderer/Renderable.h>
namespace Fay
{
	class Group : public Renderable
	{
	private:
		std::vector<Renderable*> m_renderables;
		Mat4 m_transformationMatrix;

	public:
		Group(const Mat4& transform);
		~Group();
		void add(Renderable* renderable);
		void submit(Renderer* renderer) const override;
	};
}