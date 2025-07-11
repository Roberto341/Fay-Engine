#include <Renderer/SimpleRenderer.h>
namespace Fay
{
	void SimpleRenderer::submit(const Renderable2D* renderable)
	{
		m_renderQueue.push_back((StaticSprite*)renderable);
	}
	void SimpleRenderer::flush()
	{
		while (!m_renderQueue.empty())
		{
			const StaticSprite* sprite = m_renderQueue.front();

			sprite->getVao()->bind();
			sprite->getIbo()->bind();

			sprite->getShader().setUniformMat4("ml_matrix", Mat4::translation(sprite->getPosition()));

			glDrawElements(GL_TRIANGLES, sprite->getIbo()->getCount(), GL_UNSIGNED_SHORT, nullptr);

			sprite->getVao()->unbind();
			sprite->getIbo()->unbind();

			m_renderQueue.pop_front();
		}
	}
}