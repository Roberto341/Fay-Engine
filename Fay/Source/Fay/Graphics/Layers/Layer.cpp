#include <Graphics/Layers/Layer.h>

namespace Fay
{
	Layer::Layer(Renderer* renderer, Shader* shader, Mat4 projectionMatrix, Mat4 viewMatrix)
		: m_Renderer(renderer), m_shader(shader), m_projectionMatrix(projectionMatrix), m_viewMatrix(viewMatrix)
	{
		if (m_shader) {
			m_shader->enable();
			m_shader->setUniformMat4("pr_matrix", m_projectionMatrix);
			m_shader->setUniformMat4("vw_matrix", m_viewMatrix);
			m_shader->disable();
		}
		else {
			std::cerr << "[Layer] Warning: Shader is null in constructor.\n";
		}
	}
	Layer::~Layer()
	{
		delete m_shader;
		delete m_Renderer;
		for (int i = 0; i < m_Renderables.size(); i++)
			delete m_Renderables[i];
	}

	void Layer::add(Renderable* renderable)
	{
		//std::cout << "Adding sprite with ID: " << dynamic_cast<Sprite*>(renderable)->getEntityId() << std::endl;
		m_Renderables.push_back(renderable);
	}
	void Layer::render()
	{
		m_shader->enable();

		m_Renderer->begin();
		int i = 0;

		for (const Renderable* renderables : m_Renderables)
			renderables->submit(m_Renderer);

		m_Renderer->end();
		m_Renderer->flush();
	}
	void Layer::clear()
	{
		m_Renderables.clear();
	}
}