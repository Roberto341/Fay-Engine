#pragma once
#include <iostream>
#include <vector>
#include <glew.h>
#include <Utils/FileUtils.h>
#include <Math/Math.h>

namespace Fay
{
	class Shader
	{
	private:
		const char* m_vertPath;
		const char* m_fragPath;
	public:
		GLuint m_shaderId;
		Shader(const char* vertPath, const char* fragPath);
		~Shader();

		void setUniform1f(const GLchar* name, float value);
		void setUniform1fv(const GLchar* name, float* value, int count);
		void setUniform1i(const GLchar* name, int value);
		void setUniform1iv(const GLchar* name, int* value, int count);
		void setUniform2f(const GLchar* name, const Vec2& vector);
		void setUniform3f(const GLchar* name, const Vec3& vector);
		void setUniform4f(const GLchar* name, const Vec4& vector);
		void setUniformMat4(const GLchar* name, const Mat4& matrix);

		void enable() const;
		void disable() const;

	private:
		GLuint load();
		GLint getUniformLocation(const GLchar* name);
	};
}