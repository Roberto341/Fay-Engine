#include <Graphics/Shader.h>

namespace Fay
{
	Shader::Shader(const char* vertPath, const char* fragPath)
		: m_vertPath(vertPath), m_fragPath(fragPath)
	{
		m_shaderId = load();
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_shaderId);
	}
	GLint Shader::getUniformLocation(const GLchar* name)
	{
		return glGetUniformLocation(m_shaderId, name);
	}
	void Shader::setUniform1f(const GLchar* name, float value)
	{
		glUniform1f(getUniformLocation(name), value);
	}
	void Shader::setUniform1fv(const GLchar* name, float* value, int count)
	{
		glUniform1fv(getUniformLocation(name), count, value);
	}
	void Shader::setUniform1i(const GLchar* name, int value)
	{
		glUniform1i(getUniformLocation(name), value);
	}
	void Shader::setUniform1iv(const GLchar* name, int* value, int count)
	{
		glUniform1iv(getUniformLocation(name), count, value);
	}
	void Shader::setUniform2f(const GLchar* name, const Vec2& vector)
	{
		glUniform2f(getUniformLocation(name), vector.x, vector.y);
	}
	void Shader::setUniform3f(const GLchar* name, const Vec3& vector)
	{
		glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);

	}
	void Shader::setUniform4f(const GLchar* name, const Vec4& vector)
	{
		glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}
	void Shader::setUniformMat4(const GLchar* name, const Mat4& matrix)
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.data());
	}
	/*Load and create the shader*/
	GLuint Shader::load()
	{
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertSourceString = read_file(m_vertPath);
		std::string fragSourceString = read_file(m_fragPath);

		const char* vertSource = vertSourceString.c_str();
		const char* fragSource = fragSourceString.c_str();


		glShaderSource(vertex, 1, &vertSource, NULL);
		glCompileShader(vertex);

		GLint result;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint length;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(vertex, length, &length, &error[0]);
			std::cout << "Failed to compile vertex shader" << std::endl << &error[0] << std::endl;
			glDeleteShader(vertex);
			return 0;
		}
		glShaderSource(fragment, 1, &fragSource, NULL);
		glCompileShader(fragment);


		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint length;
			glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(fragment, length, &length, &error[0]);
			std::cout << "Failed to compile fagment shader" << std::endl << &error[0] << std::endl;
			glDeleteShader(fragment);
			return 0;
		}

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);

		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return program;
	}
	/*Enable*/
	void Shader::enable() const
	{
		glUseProgram(m_shaderId);
	}
	/*Disable */
	void Shader::disable() const
	{
		glUseProgram(0);
	}
}