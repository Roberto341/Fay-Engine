#pragma once
#include <Renderer/Renderable.h>
#include <Renderer/Renderer.h>
namespace Fay
{
	class Layer
	{
	protected:
		Renderer* m_Renderer;

		std::vector<Renderable*> m_Renderables;
		Shader* m_shader;
		Mat4 m_projectionMatrix;
		Mat4 m_viewMatrix;
	protected:
		Layer(Renderer* renderer, Shader* shader, Mat4 projectionMatrix, Mat4 viewMatrix);


	public:
		virtual ~Layer();
		virtual void add(Renderable* renderable);
		virtual void remove(Renderable* renderable);
		virtual void render();
		virtual void clear();
	};
}