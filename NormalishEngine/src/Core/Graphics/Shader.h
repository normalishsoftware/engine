#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL\glew.h>


class Shader
{
public:

	static void Shader::LoadShader(const char* file_path, std::string* vertex_source, std::string* fragment_source)
	{
		enum class ShaderType
		{
			NONE = -1,
			VERTEX,
			FRAGMENT
		};

		std::ifstream file(file_path);
		std::stringstream ss[2];
		ShaderType shader_type = ShaderType::NONE;

		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
			{
				if (line.find("#shader") != std::string::npos)
				{
					if (line.find("vertex") != std::string::npos)
						shader_type = ShaderType::VERTEX;
					else if (line.find("fragment") != std::string::npos)
						shader_type = ShaderType::FRAGMENT;
				}
				else
					ss[static_cast<int32_t>(shader_type)] << line << '\n';
			}
		}
		*vertex_source = ss[0].str();
		*fragment_source = ss[1].str();
	}

	static uint32_t Shader::CompileShader(uint32_t type, const char* source)
	{
		uint32_t id = glCreateShader(type);
		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		{
			int32_t result = 0;
			glGetShaderiv(id, GL_COMPILE_STATUS, &result);
			if (!result)
			{
				int32_t length = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
				char* error = static_cast<char*>(alloca(length * sizeof(char)));
				glGetShaderInfoLog(id, length, &length, error);
				std::cout << error << '\n';

				glDeleteShader(id);
				return 0;
			}
		}

		return id;
	}

	static uint32_t Shader::CreateShader(const char* vertex_shader, const char* fragment_shader)
	{
		uint32_t program = glCreateProgram();
		uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertex_shader);
		uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
};
