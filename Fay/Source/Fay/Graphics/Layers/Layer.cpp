#include <Graphics/Layers/Layer.h>

namespace Fay
{
	Layer::Layer(Renderer2D* renderer, Shader* shader, Mat4 projectionMatrix)
		: m_renderer(renderer), m_3dRenderer(nullptr), m_shader(shader), m_projectionMatrix(projectionMatrix)
	{
		m_shader->enable();
		m_shader->setUniformMat4("pr_matrix", m_projectionMatrix);
		m_shader->disable();
	}

	Layer::Layer(Renderer3D* renderer, Shader* shader, Mat4 projectionMatrix, Mat4 viewMatrix)
		:m_renderer(nullptr), m_3dRenderer(renderer), m_shader(shader),m_projectionMatrix(projectionMatrix), m_viewMatrix(viewMatrix)
	{
		m_shader->enable();
		m_shader->setUniformMat4("pr_matrix", m_projectionMatrix);
		m_shader->setUniformMat4("vw_matrix", m_viewMatrix);
		m_shader->disable();
	}
	Layer::~Layer()
	{
		delete m_shader;
		delete m_renderer;
		delete m_3dRenderer;

		for (int i = 0; i < m_renderAbles.size(); i++)
			delete m_renderAbles[i];
		for (int j = 0; j < m_3dRenderables.size(); j++)
			delete m_3dRenderables[j];
	}

	void Layer::add(Renderable2D* renderable)
	{
		m_renderAbles.push_back(renderable);
	}
	void Layer::add(Renderable3D* renderable)
	{
		m_3dRenderables.push_back(renderable);
	}
	void Layer::render()
	{
		m_shader->enable();

		m_renderer->begin();
		int i = 0;

		for (const Renderable2D* renderables : m_renderAbles)
			renderables->submit(m_renderer);

		m_renderer->end();
		m_renderer->flush();
	}
	void Layer::render3d()
	{
		m_shader->enable();

		m_3dRenderer->begin();

		int i = 0;

		for (const Renderable3D* d3renderables : m_3dRenderables)
			d3renderables->submit(m_3dRenderer);

		m_3dRenderer->end();
		m_3dRenderer->flush();
	}
	void Layer::clear()
	{
		/*for (auto* renderables : m_renderAbles)
		{
			delete renderables;
		}*/
		m_3dRenderables.clear();
		m_renderAbles.clear();
	}
}