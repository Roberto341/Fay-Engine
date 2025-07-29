#pragma once
#include <glew.h>
#include <iostream>
namespace Fay
{
	class FrameBuffer
	{
	public: 
		FrameBuffer(int width, int height);
		~FrameBuffer();

		void bind() const;
		void unbind(int windowWidth, int windowHeight) const;
		void resize(uint32_t width, uint32_t height);
		GLuint getTexture() const { return m_texture; }
		inline int getWidth() const { return m_width; }
		inline int getHeight() const { return m_height; }
	private:
		int m_width, m_height;
		GLuint m_fbo;
		GLuint m_texture;
		//GLuint m_rbo; // optional for depth/stencil
	};
}