#pragma once
#include <Renderer/Renderer2d.h>
#include <Renderer/Renderable2d.h>
namespace Fay
{
	class Layer
	{
	protected:
		Renderer2D* m_renderer;
		std::vector<Renderable2D*> m_renderAbles;
		Shader* m_shader;
		Mat4 m_projectionMatrix;
	protected:
		Layer(Renderer2D* renderer, Shader* shader, Mat4 projectionMatrix);

	public:
		virtual ~Layer();
		virtual void add(Renderable2D* renderable);
		virtual void render();
		virtual void clear();
	};
}