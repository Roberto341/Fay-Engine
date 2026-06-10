#include <Graphics/Layers/Layer.h>

namespace Fay
{
	Layer::Layer(Renderer* renderer, Shader* shader, Mat4 projectionMatrix, Mat4 viewMatrix)
		: m_Renderer(renderer), m_shader(shader), m_projectionMatrix(projectionMatrix), m_viewMatrix(viewMatrix)
	{
		setShader(new Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag"));
		if (!m_shader)
			FAY_LOG_ERROR("[Layer] Warning: Shader is null in constructor");
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
		m_Renderables.push_back(renderable);
	}
	void Layer::remove(Renderable* renderable)
	{
		auto it = std::find(m_Renderables.begin(), m_Renderables.end(), renderable);
		if (it != m_Renderables.end())
		{
			m_Renderables.erase(it);
		}
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

	void Layer::setProjectionType(ProjectionType type)
	{
		m_ptype = type;
	}
	void Layer::setShader(Shader* shader)
	{
		m_shader = shader;
	}
}