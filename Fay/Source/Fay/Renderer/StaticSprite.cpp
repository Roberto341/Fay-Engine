#include <Renderer/StaticSprite.h>

namespace Fay
{
	StaticSprite::StaticSprite(float x, float y, float width, float height, const Vec4& color, Shader& shader)
		: Renderable2D(Vec3(x, y, 0), Vec2(width, height), color), m_shader(shader)
	{
		m_vertexArray = new VertexArray();

		GLfloat verts[] =
		{
			0, 0, 0,
			0, height, 0,
			width, height, 0,
			width, 0, 0
		};

		GLfloat colors[] =
		{
			color.x, color.y, color.z, color.w,
			color.x, color.y, color.z, color.w,
			color.x, color.y, color.z, color.w,
			color.x, color.y, color.z, color.w
		};

		m_vertexArray->addBuffer(new Buffer(verts, 4 * 3, 3), 0);
		m_vertexArray->addBuffer(new Buffer(colors, 4 * 4, 4), 1);

		GLushort indicies[] = { 0, 1, 2, 2, 3, 0 };
		m_indexBuffer = new IndexBuffer(indicies, 6);
	}

	StaticSprite::~StaticSprite()
	{
		delete m_vertexArray;
		delete m_indexBuffer;
	}
}