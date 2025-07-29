#include <Renderer/3DRenderer/BatchRenderer3D.h>

namespace Fay
{
		BatchRenderer3D::BatchRenderer3D()
		{
			init();
		}

		BatchRenderer3D::~BatchRenderer3D()
		{
			delete m_ibo;
			glDeleteBuffers(1, &m_vbo);
		}

		void BatchRenderer3D::init()
		{
			//m_indexCount = 0;
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);
			
			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, D3_RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(D3_SHADER_VERTEX_INDEX);
			glEnableVertexAttribArray(D3_SHADER_UV_INDEX);
			glEnableVertexAttribArray(D3_SHADER_TID_INDEX);
			glEnableVertexAttribArray(D3_SHADER_COLOR_INDEX);

			glVertexAttribPointer(D3_SHADER_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, D3_RENDERER_VERTEX_SIZE, (const GLvoid*)0);
			glVertexAttribPointer(D3_SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, D3_RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(D3VertexData, D3VertexData::uv)));
			glVertexAttribPointer(D3_SHADER_TID_INDEX, 1, GL_FLOAT, GL_FALSE, D3_RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(D3VertexData, D3VertexData::tid)));
			glVertexAttribPointer(D3_SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, D3_RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(D3VertexData, D3VertexData::color)));

			glBindBuffer(GL_ARRAY_BUFFER, 0);


			GLuint* indicies = new GLuint[D3_RENDERER_INDICES_SIZE];

			int cubeCount = D3_RENDERER_INDICES_SIZE / 36;

			for (int cube = 0; cube < cubeCount; cube++)
			{
				int vertexOffset = cube * 24;
				int indexOffset = cube * 36;

				for (int face = 0; face < 6; face++)
				{
					int faceVertexOffset = vertexOffset + face * 4;
					int faceIndexOffset = indexOffset + face * 6;

					indicies[faceIndexOffset + 0] = faceVertexOffset + 0;
					indicies[faceIndexOffset + 1] = faceVertexOffset + 1;
					indicies[faceIndexOffset + 2] = faceVertexOffset + 2;
					indicies[faceIndexOffset + 3] = faceVertexOffset + 2;
					indicies[faceIndexOffset + 4] = faceVertexOffset + 3;
					indicies[faceIndexOffset + 5] = faceVertexOffset + 0;
				}
			}
			m_ibo = new IndexBuffer(indicies, D3_RENDERER_INDICES_SIZE);
			m_ibo->bind();
			glBindVertexArray(0);
		}

		void BatchRenderer3D::begin()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			m_buffer = (D3VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		}

		void BatchRenderer3D::submit(const Renderable3D* renderable)
		{
			const Vec3& position = renderable->getPosition();
			const Vec3& size = renderable->getSize();
			const Vec4& color = renderable->getColor();
			const std::vector<Vec2>& uv = renderable->getUV();
			const GLuint tid = renderable->getTID();

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

			unsigned int c = ((int)(color.w * 255.0f) << 24) |
				((int)(color.z * 255.0f) << 16) |
				((int)(color.y * 255.0f) << 8) |
				((int)(color.x * 255.0f));

			// Use a flaot quad in 3D space (z-facing)
			Vec3 pos = position;
			Vec3 s = size;

			Vec3 cubeVertices[6][4] = {
				// Front face
			{	Vec3(0, 0, 0) * size + position,
				Vec3(0, 1, 0) * size + position,
				Vec3(1, 1, 0) * size + position,
				Vec3(1, 0, 0) * size + position
			},
				// Back face
			{
				Vec3(0, 0, 1) * size + position,
				Vec3(0, 1, 1) * size + position,
				Vec3(1, 1, 1)* size + position,
				Vec3(1, 0, 1)* size + position
			},
				// Left face
			{	 
				Vec3(0, 0, 1) * size + position,
				Vec3(0, 1, 1)* size + position,
				Vec3(0, 1, 0)* size + position,
				Vec3(0, 0, 0)* size + position
			},
				// Right face
			{	Vec3(1, 0, 0) * size + position,
				Vec3(1, 1, 0) * size + position,
				Vec3(1, 1, 1) * size + position,
				Vec3(1, 0, 1) * size + position
			},
				// Top face
			{	Vec3(0, 1, 0) * size + position,
				Vec3(0, 1, 1) * size + position,
				Vec3(1, 1, 1) * size + position,
				Vec3(1, 1, 0) * size + position
			},
				// Bottom face
				{	Vec3(0, 0, 1) * size + position,
					Vec3(0, 0, 0) * size + position,
					Vec3(1, 0, 0) * size + position,
					Vec3(1, 0, 1) * size + position
				}
			};
			Vec2 faceUVs[4] = {
				Vec2(0, 0),
				Vec2(0, 1),
				Vec2(1, 1),
				Vec2(1, 0)
			};
			for (int face = 0; face < 6; face++)
			{
				for (int i = 0; i < 4; i++)
				{
					Mat4 model = *m_transformationBack;
					//Vec3 transformed = model * cubeVertices[face][i];
					//m_buffer->vertex = transformed;
					m_buffer->vertex = model * cubeVertices[face][i];// apply full MVP transform
					m_buffer->uv = faceUVs[i];
					m_buffer->tid = ts;
					m_buffer->color = c;
					m_buffer++;
				}
			}
			//std::cout << "Transformed vertex: " << m_buffer->vertex.x << ", " << m_buffer->vertex.y << ", " << m_buffer->vertex.z << "\n";
			m_indexCount += 36;
		}
		void BatchRenderer3D::end()
		{
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void BatchRenderer3D::flush()
		{
			for (int i = 0; i < m_textureSlots.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, m_textureSlots[i]);
			}
			glBindVertexArray(m_vao); // bind
			//m_ibo->bind();

			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL); // draw to the screen

			//m_ibo->unbind(); // unbind
			glBindVertexArray(0);

			m_indexCount = 0; // reset the index count
		}
	}