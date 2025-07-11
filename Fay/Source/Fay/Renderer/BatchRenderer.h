#pragma once
#include <cstddef>
#include <Renderer/Renderer2d.h>
#include <Renderer/Renderable2d.h>
#include <Graphics/Buffers/IndexBuffer.h>

namespace Fay
{
#define RENDERER_MAX_SPRITES	60000
#define RENDERER_VERTEX_SIZE	sizeof(VertexData)
#define RENDERER_SPRITE_SIZE	RENDERER_VERTEX_SIZE * 4
#define RENDERER_BUFFER_SIZE	RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE	RENDERER_MAX_SPRITES * 6
#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX		1
#define SHADER_TID_INDEX	2
#define SHADER_COLOR_INDEX	3	

	class BatchRenderer : public Renderer2D
	{
	private:
		GLuint m_vao;
		GLuint m_vbo;
		IndexBuffer* m_ibo;
		GLsizei m_indexCount;
		VertexData* m_buffer;

		std::vector<GLuint> m_textureSlots;

	public:
		BatchRenderer();
		~BatchRenderer();
		void begin() override;
		void submit(const Renderable2D* renderable) override;
		void drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color) override;
		void end() override;
		void flush() override;
	private:
		void init();
	};
}