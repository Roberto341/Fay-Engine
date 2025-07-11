#pragma once

#include <Graphics/Buffers/Buffer.h>
#include <Graphics/Buffers/IndexBuffer.h>
#include <Graphics/Buffers/VertexArray.h>
#include <Renderer/Renderer2d.h>
#include <Graphics/Texture.h>
#include <Math/Math.h>
#include <Graphics/Shader.h>

namespace Fay
{
	struct VertexData
	{
		Vec3 vertex;
		Vec2 uv;
		float tid;
		unsigned int color;
	};

	class Renderable2D
	{
	protected:
		Vec3 m_position;
		Vec2 m_size;
		Vec4 m_color;
		std::vector<Vec2> m_uv;
		Texture* m_texture;
	protected:
		Renderable2D() : m_texture(nullptr) { setUVDefaults(); }

	public:
		Renderable2D(Vec3 position, Vec2 size, Vec4 color)
			: m_position(position), m_size(size), m_color(color), m_texture(nullptr)
		{
			setUVDefaults();
		}
		virtual ~Renderable2D(){ }

		virtual void submit(Renderer2D* renderer) const
		{
			renderer->submit(this);
		}

		inline const Vec3& getPosition() const { return m_position; } // position of the shape
		inline const Vec2& getSize() const { return m_size; } // size of the shape
		inline const Vec4& getColor() const { return m_color; } // color of the shape 
		inline const std::vector<Vec2>& getUV() const { return m_uv; } // uv of the shape
		inline const GLuint getTID() const { return m_texture ? m_texture->getId() : 0; }
	private:
		void setUVDefaults() {
			m_uv.push_back(Vec2(0, 0));
			m_uv.push_back(Vec2(0, 1));
			m_uv.push_back(Vec2(1, 1));
			m_uv.push_back(Vec2(1, 0));
		}
	};
}