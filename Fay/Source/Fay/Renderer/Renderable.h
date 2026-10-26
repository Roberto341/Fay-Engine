#pragma once

#include <Graphics/Buffers/Buffer.h>
#include <Graphics/Buffers/IndexBuffer.h>
#include <Graphics/Buffers/VertexArray.h>
#include <Renderer/Renderer.h>
#include <Graphics/Texture.h>
#include <Math/Math.h> 
#include <Graphics/Shader.h>

namespace Fay
{
	struct TestVertexData
	{
		Vec3 vertex;
		Vec2 uv;
		float tid;
		unsigned int color;
	};
	enum class RenderDimension { D2, D3 };
	class Renderable
	{
	protected:
		uint32_t m_id;
		Vec3 m_position;
		Vec3 m_size;
		Vec4 m_color;
		std::vector<Vec2> m_uv;
		bool m_collision;
		Texture* m_texture;
		RenderDimension m_dimension = RenderDimension::D2;
	protected:
		Renderable() : m_texture(nullptr) { setUVDefaults(); }
	public:
		Renderable(uint32_t id, Vec3 position, Vec3 size, Vec4 color, RenderDimension dimension = RenderDimension::D2)
			:m_id(id), m_position(position), m_size(size), m_color(color), m_texture(nullptr), m_dimension(dimension)
		{
			setUVDefaults();
		}
		virtual ~Renderable() {}
		virtual void submit(Renderer* renderer) const
		{
			renderer->submit(this);
		}
		void setCollision(bool cond)
		{
			m_collision = cond;
		}
		inline const uint32_t& getId() const { return m_id; }
		inline const Vec3& getPosition() const { return m_position; }
		inline const Vec3& getSize() const { return m_size; }
		inline const Vec4& getColor() const { return m_color; }
		inline const bool& getCollision() const { return m_collision; }
		inline const std::vector<Vec2>& getUV() const { return m_uv; }
		inline const GLuint getTID() const { return m_texture ? m_texture->getId() : 0; }
		void setPosition(Vec3 position) {
			m_position = position;
		}
		void setSize(Vec3 size)
		{
			m_size = size;
		}
		void setTexture(Texture* texture)
		{
			m_texture = texture;
		}
		void setColor(Vec4 color)
		{
			m_color = color;
		}
		Texture* getTexture() const { return m_texture; }

		RenderDimension getDimension() const { return m_dimension; }

		bool checkCollision(const Renderable* other, bool useZ = false)
		{
			bool collision =
				m_position.x < other->m_position.x + other->m_size.x &&
				other->m_position.x < m_position.x + m_size.x &&
				m_position.y < other->m_position.y + other->m_size.y &&
				other->m_position.y < m_position.y + m_size.y;

			if (useZ)
			{
				collision = collision &&
					m_position.z < other->m_position.z + other->m_size.z &&
					other->m_position.z < m_position.z + m_size.z;
			}

			return collision;
		}

		// Animation stuff
		void setUV(const std::vector<Vec2>& uv)
		{
			if (uv.size() != 4)
				return;

			m_uv = uv;
		}

		void setSpriteFrame(int frame, int frameWidth, int frameHeight)
		{
			if (frameWidth <= 0 || frameHeight <= 0)
				return;

			const int textureWidth = m_texture->getWidth();
			const int textureHeight = m_texture->getHeight();

			int cols = textureWidth / frameWidth;
			int rows = textureHeight / frameHeight;

			if (cols <= 0 || rows <= 0 || frame < 0 || frame >= cols * rows)
				return;

			int col = frame % cols;
			int row = frame / cols;

			float u0 = static_cast<float>(col * frameWidth) / textureWidth;
			float u1 = static_cast<float>((col + 1) * frameWidth) / textureWidth;

			// Image rows are normally counted from the top.
			// OpenGL texture UVs usually have their origin at the bottom.
			float v1 = 1.0f - static_cast<float>(row * frameHeight) / textureHeight;
			float v0 = 1.0f - static_cast<float>((row + 1) * frameHeight) / textureHeight;

			m_uv = {
				Vec2(u0, v0),
				Vec2(u0, v1),
				Vec2(u1, v1),
				Vec2(u1, v0)
			};
		}

	private:
		void setUVDefaults()
		{
			m_uv.push_back(Vec2(0, 0));
			m_uv.push_back(Vec2(0, 1));
			m_uv.push_back(Vec2(1, 1));
			m_uv.push_back(Vec2(1, 0));
		}
	};
}