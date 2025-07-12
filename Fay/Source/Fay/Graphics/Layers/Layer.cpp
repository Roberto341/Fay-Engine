#include <Graphics/Layers/Layer.h>

namespace Fay
{
	Layer::Layer(Renderer2D* renderer, Shader* shader, Mat4 projectionMatrix)
		: m_renderer(renderer), m_shader(shader), m_projectionMatrix(projectionMatrix)
	{
		m_shader->enable();
		m_shader->setUniformMat4("pr_matrix", m_projectionMatrix);
		m_shader->disable();
	}

	Layer::~Layer()
	{
		delete m_shader;
		delete m_renderer;

		for (int i = 0; i < m_renderAbles.size(); i++)
			delete m_renderAbles[i];
	}

	void Layer::add(Renderable2D* renderable)
	{
		m_renderAbles.push_back(renderable);
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

	void Layer::clear()
	{
		for (auto* renderables : m_renderAbles)
		{
			delete renderables;
		}
		m_renderAbles.clear();
	}
}