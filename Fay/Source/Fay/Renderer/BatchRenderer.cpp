#include <Renderer/BatchRenderer.h>

namespace Fay
{
	BatchRenderer::BatchRenderer()
	{
		init();
	}

	BatchRenderer::~BatchRenderer()
	{
		delete m_ibo;
		glDeleteBuffers(1, &m_vbo);
	}

	void BatchRenderer::init()
	{
		glGenVertexArrays(1, &m_vao);
		glGenVertexArrays(1, &m_vbo);
		glBindVertexArray(m_vao);
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
		glEnableVertexAttribArray(SHADER_UV_INDEX);
		glEnableVertexAttribArray(SHADER_TID_INDEX);
		glEnableVertexAttribArray(SHADER_COLOR_INDEX);

		glVertexAttribPointer(SHADER_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)0);
		glVertexAttribPointer(SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::uv)));
		glVertexAttribPointer(SHADER_TID_INDEX, 1, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::tid)));
		glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(VertexData, VertexData::color)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	

		GLuint* indicies = new GLuint[RENDERER_INDICES_SIZE];

		int offset = 0;
		for (int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
		{
			indicies[i] = offset + 0;
			indicies[i + 1] = offset + 1;
			indicies[i + 2] = offset + 2;
			indicies[i + 3] = offset + 2;
			indicies[i + 4] = offset + 3;
			indicies[i + 5] = offset + 0;
			
			offset += 4;
		}
		m_ibo = new IndexBuffer(indicies, RENDERER_INDICES_SIZE);
		glBindVertexArray(0);
	}

	void BatchRenderer::begin()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		m_buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	void BatchRenderer::submit(const Renderable2D* renderable)
	{
		const Vec3& position = renderable->getPosition();
		const Vec2& size = renderable->getSize();
		const Vec4& color = renderable->getColor();
		const std::vector<Vec2>& uv = renderable->getUV();
		const GLuint tid = renderable->getTID();

		unsigned int c = 0;

		float ts = 0.0f;
		if (tid > 0)
		{
			bool found = false;
			for (int i = 0; i < m_textureSlots.size(); i++)
			{
				if (m_textureSlots[i] == tid)
				{
					ts = (float)(i + 1);
					found = true;
					break;
				}
			}

			if (!found)
			{
				if (m_textureSlots.size() >= 16)
				{
					end();
					flush();
					begin();
				}
				m_textureSlots.push_back(tid);
				ts = (float)(m_textureSlots.size());
			}
		}

		int r = color.x * 255.0f;
		int g = color.y * 255.0f;
		int b = color.z * 255.0f;
		int a = color.w * 255.0f;

		c = a << 24 | b << 16 | g << 8 | r;

		m_buffer->vertex = *m_transBack * position;
		m_buffer->uv = uv[0];
		m_buffer->tid = ts;
		m_buffer->color = c;
		m_buffer++;

		m_buffer->vertex = *m_transBack * Vec3(position.x, position.y + size.y, position.z);
		m_buffer->uv = uv[1];
		m_buffer->tid = ts;
		m_buffer->color = c;
		m_buffer++;

		m_buffer->vertex = *m_transBack * Vec3(position.x + size.x, position.y + size.y, position.z);
		m_buffer->uv = uv[2];
		m_buffer->tid = ts;
		m_buffer->color = c;
		m_buffer++;

		m_buffer->vertex = *m_transBack * Vec3(position.x + size.x, position.y, position.z);
		m_buffer->uv = uv[3];
		m_buffer->tid = ts;
		m_buffer->color = c;
		m_buffer++;

		m_indexCount += 6;
	}

	void BatchRenderer::drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color)
	{
		using namespace ftgl;

		int r = color.x * 255.0f;
		int g = color.y * 255.0f;
		int b = color.z * 255.0f;
		int a = color.w * 255.0f;

		unsigned int col = a << 24 | b << 16 | g << 8 | r;

		float ts = 0.0f;
		bool found = false;
		for (int i = 0; i < m_textureSlots.size(); i++)
		{
			if (m_textureSlots[i] == font.getID())
			{
				ts = (float)(i + 1);
				found = true;
				break;
			}
		}

		if (!found)
		{
			if (m_textureSlots.size() >= 16)
			{
				end();
				flush();
				begin();
			}
			m_textureSlots.push_back(font.getID());
			ts = (float)(m_textureSlots.size());
		}

		float scaleX = 960.0f / 32.0f;
		float scaleY = 540.0f / 18.0f;

		float x = position.x;

		texture_font_t* ftFont = font.getFTGLFont();
		for (int i = 0; i < text.length(); i++)
		{
			char c = text[i];
			texture_glyph_t* glyph = texture_font_get_glyph(font.getFTGLFont(), c);
			if (glyph != NULL)
			{
				if (i > 0)
				{
					float kerning = texture_glyph_get_kerning(glyph, text[i - 1]);
					x += kerning / scaleX;
				}
				float x0 = x + glyph->offset_x / scaleX;
				float y0 = position.y + glyph->offset_y / scaleY;
				float x1 = x0 + glyph->width / scaleX;
				float y1 = y0 - glyph->height / scaleY;

				float u0 = glyph->s0;
				float v0 = glyph->t0;
				float u1 = glyph->s1;
				float v1 = glyph->t1;

				m_buffer->vertex = *m_transBack * Vec3(x0, y0, 0);
				m_buffer->uv = Vec2(u0, v0);
				m_buffer->tid = ts;
				m_buffer->color = col;
				m_buffer++;

				m_buffer->vertex = *m_transBack * Vec3(x0, y1, 0);
				m_buffer->uv = Vec2(u0, v1);
				m_buffer->tid = ts;
				m_buffer->color = col;
				m_buffer++;

				m_buffer->vertex = *m_transBack * Vec3(x1, y1, 0);
				m_buffer->uv = Vec2(u1, v1);
				m_buffer->tid = ts;
				m_buffer->color = col;
				m_buffer++;

				m_buffer->vertex = *m_transBack * Vec3(x1, y0, 0);
				m_buffer->uv = Vec2(u1, v0);
				m_buffer->tid = ts;
				m_buffer->color = col;
				m_buffer++;

				m_indexCount += 6;

				x += glyph->advance_x / scaleX;
			}
		}
	}

	void BatchRenderer::end()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void BatchRenderer::flush()
	{
		for (int i = 0; i < m_textureSlots.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textureSlots[i]);
		}
		glBindVertexArray(m_vao); // bind
		m_ibo->bind();

		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL); // draw to the screen

		m_ibo->unbind(); // unbind
		glBindVertexArray(0);

		m_indexCount = 0; // reset the index count
	}
}
