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