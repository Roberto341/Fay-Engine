#pragma once
#include <cstddef>
#include <Renderer/Renderer.h>
#include <Renderer/Renderable.h>
#include <Graphics/Buffers/IndexBuffer.h>

namespace Fay
{
#define RENDERER_MAX_SPRITES	60000
#define RENDERER_VERTEX_SIZE	sizeof(TestVertexData)
#define RENDERER_SPRITE_SIZE	RENDERER_VERTEX_SIZE * 4 //24 for 2d cube
#define RENDERER_BUFFER_SIZE	RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE	RENDERER_MAX_SPRITES * 6// 36 for cube
#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX		1
#define SHADER_TID_INDEX	2
#define SHADER_COLOR_INDEX	3
#define RENDERER_CUBE_SIZE	    RENDERER_VERTEX_SIZE * 24 //4 for 2d quad
#define RENDERER_CUBE_BUFFER_SIZE    RENDERER_CUBE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_CUBE_SIZE	RENDERER_MAX_SPRITES * 36// 36 for cube

	class BatchRenderer : public Renderer
	{
	private:
		GLuint m_vao;
		GLuint m_vbo;
		IndexBuffer* m_ibo;
		GLsizei m_indexCount;
		TestVertexData* m_buffer;

		std::vector<GLuint> m_textureSlots;
		RenderDimension m_dimension = RenderDimension::D2; // defaults to 2d
	public:
		BatchRenderer();
		~BatchRenderer();
		void begin() override;
		void submit(const Renderable* renderable) override;
		void drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color) override;
		void end() override;
		void flush() override;
		void setDimension(RenderDimension dimension);
	private:
		void init();
	};
}