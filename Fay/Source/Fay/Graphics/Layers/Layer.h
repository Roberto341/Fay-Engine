#pragma once
#include <Renderer/Renderer2d.h>
#include <Renderer/Renderable2d.h>
#include <Renderer/3DRenderer/Renderer3d.h>
#include <Renderer/3DRenderer/Renderable3d.h>
namespace Fay
{
	class Layer
	{
	protected:
		Renderer2D* m_renderer;
		Renderer3D* m_3dRenderer;

		std::vector<Renderable2D*> m_renderAbles;
		std::vector<Renderable3D*> m_3dRenderables;
		Shader* m_shader;
		Mat4 m_projectionMatrix;
		Mat4 m_viewMatrix;
	protected:
		Layer(Renderer2D* renderer, Shader* shader, Mat4 projectionMatrix);
		Layer(Renderer3D* renderer, Shader* shader, Mat4 projectionMatrix, Mat4 viewMatrix);
	public:
		virtual Renderer3D* getRenderer() { return m_3dRenderer; }
		virtual ~Layer();
		virtual void add(Renderable2D* renderable);
		virtual void add(Renderable3D* renderable);
		virtual void render();
		virtual void render3d();
		virtual void clear();
	};
}