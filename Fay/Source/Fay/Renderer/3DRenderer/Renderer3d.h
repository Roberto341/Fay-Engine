#pragma once

#include <vector>
#include <glew.h>
#include <Math/Math.h>
#include <Graphics/Font/Font.h>

namespace Fay {
		class Renderable3D;
		class Renderer3D
		{
		protected: 
			std::vector<Mat4> m_transformationStack;
			const Mat4* m_transformationBack;

			Mat4 m_viewMatrix = Mat4::identity();
			Mat4 m_projMatrix = Mat4::identity();
		protected:
			Renderer3D()
			{
				m_transformationStack.push_back(Mat4::identity());
				m_transformationBack = &m_transformationStack.back();
			}
		public:
			void push(const Mat4& matrix, bool override = false)
			{
				if (override)
					m_transformationStack.push_back(matrix);
				else
					m_transformationStack.push_back(m_transformationStack.back() * matrix);
				m_transformationBack = &m_transformationStack.back();
			}
			void pop()
			{
				if (m_transformationStack.size() > 1)
					m_transformationStack.pop_back();

				m_transformationBack = &m_transformationStack.back();
			}
			void setViewMatrix(const Mat4& view) { m_viewMatrix = view; }
			void setProjectionMatrix(const Mat4& proj) { m_projMatrix = proj; }
			
			const Mat4& getViewMatrix() const { return m_viewMatrix; }
			const Mat4& getProjectionMatrix() const { return m_projMatrix; }

			virtual void begin() {} 
			virtual void submit(const Renderable3D* renderable) = 0;
			virtual void drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color) {}
			virtual void end() {}
			virtual void flush() = 0;
		};
	}