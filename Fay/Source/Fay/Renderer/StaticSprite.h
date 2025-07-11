#pragma once
#include <Renderer/Renderable2d.h>

namespace Fay
{
	class StaticSprite : public Renderable2D
	{
	private:
		VertexArray* m_vertexArray;
		IndexBuffer* m_indexBuffer;
		Shader& m_shader;

	public:
		StaticSprite(float x, float y, float width, float height, const Vec4& color, Shader& shader);
		~StaticSprite();

		inline const VertexArray* getVao() const { return m_vertexArray; }
		inline const IndexBuffer* getIbo() const { return m_indexBuffer; }

		inline Shader& getShader() const { return m_shader;  }
	};
}