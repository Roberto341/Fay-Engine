#pragma once

#include <Graphics/Buffers/Buffer.h>
#include <Graphics/Buffers/IndexBuffer.h>
#include <Graphics/Buffers/VertexArray.h>
#include <Renderer/3DRenderer/Renderer3d.h>
#include <Graphics/Texture.h>
#include <Math/Math.h>
#include <Graphics/Shader.h>

namespace Fay
{
		struct D3VertexData
		{
			Vec3 vertex;
			Vec2 uv;
			float tid;
			unsigned int color;
		};
		class Renderable3D
		{
		protected:
			Vec3 m_position;
			Vec3 m_size;
			Vec4 m_color;
			std::vector<Vec2> m_uv;
			Texture* m_texture;
		protected:
			Renderable3D() : m_texture(nullptr) { setUVDefaults(); }
		public:
			Renderable3D(Vec3 position, Vec3 size, Vec4 color)
				: m_position(position), m_size(size), m_color(color), m_texture(nullptr)
			{
				setUVDefaults();
			}
			virtual ~Renderable3D() {}
			virtual void submit(Renderer3D* renderer) const
			{
				renderer->submit(this);
			}
			inline const Vec3& getPosition() const { return m_position; }
			inline const Vec3& getSize() const { return m_size; }
			inline const Vec4& getColor() const { return m_color; }
			inline const std::vector<Vec2>& getUV() const { return m_uv; }
			inline const GLuint getTID() const { return m_texture ? m_texture->getId() : 0; }

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