#pragma once

#include <vector>
#include <glew.h>
#include <Math/Math.h>
#include <Graphics/Font/Font.h>
namespace Fay
{
	class Renderable2D;
	class Renderer2D 
	{
	protected:
		std::vector<Mat4> m_transStack;
		const Mat4* m_transBack;
	protected:
		Renderer2D()
		{
			m_transStack.push_back(Mat4::identity());
			m_transBack = &m_transStack.back();
		}
	public:
		const Mat4& getTopTransform() const
		{
			return m_transStack.back();
		}
		void push(const Mat4& matrix, bool overide = false)
		{
			if (overide)
				m_transStack.push_back(matrix);
			else
				m_transStack.push_back(m_transStack.back() * matrix);
			
			m_transBack = &m_transStack.back();
		}
		void pop()
		{
			if (m_transStack.size() > 1)
				m_transStack.pop_back();

			m_transBack = &m_transStack.back();
		}
		virtual void begin() {}
		virtual void submit(const Renderable2D* renderable) = 0;
		virtual void drawString(const std::string& text, const Vec3& position, const Font& font, const Vec4& color) { }
		virtual void end() {}
		virtual void flush() = 0;
	};
}