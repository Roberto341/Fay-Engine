#pragma once
#include <cstddef>
#include <Renderer/3DRenderer/Renderer3d.h>
#include <Renderer/3DRenderer/Renderable3d.h>
#include <Graphics/Buffers/IndexBuffer.h>

namespace Fay
{
#define D3_RENDERER_MAX_SPRITES	6000
#define D3_RENDERER_VERTEX_SIZE    sizeof(D3VertexData)
#define D3_RENDERER_SPRITE_SIZE	D3_RENDERER_VERTEX_SIZE * 24
#define D3_RENDERER_BUFFER_SIZE	D3_RENDERER_SPRITE_SIZE * D3_RENDERER_MAX_SPRITES
#define D3_RENDERER_INDICES_SIZE	D3_RENDERER_MAX_SPRITES * 36
#define D3_SHADER_VERTEX_INDEX		0
#define D3_SHADER_UV_INDEX			1
#define D3_SHADER_TID_INDEX		2
#define D3_SHADER_COLOR_INDEX		3	
		class BatchRenderer3D : public Renderer3D
		{
		private:
			GLuint m_vao;
			GLuint m_vbo;
			IndexBuffer* m_ibo;
			GLsizei m_indexCount;
			D3VertexData* m_buffer;
			std::vector<GLuint> m_textureSlots;
		public:
			BatchRenderer3D();
			~BatchRenderer3D();
			void begin() override;
			void submit(const Renderable3D* renderable) override;
			//void drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color) override; // will implement this later
			void end() override;
			void flush() override;
		private:
			void init();
		};
	}