#pragma once

#include "ogl_shader.hpp"

#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "system/io.hpp"
#include "util/macros.hpp"

#define SHADER_ERROR_LOG_SIZE 512

uint32_t ext_to_shader_type(const std::filesystem::path &path)
{
	auto ext = path.extension();

	if (ext == ".vert")
	{
		return GL_VERTEX_SHADER;
	}
	if (ext == ".frag")
	{
		return GL_FRAGMENT_SHADER;
	}

	return GL_NONE;
}

bool forge::OglShader::compile(const ShaderSource& source)
{
	m_program = glCreateProgram();

	char info_buffer[SHADER_ERROR_LOG_SIZE];

	for (auto &path : source)
	{
		if (path.empty())
		{
			continue;
		}

		UNWRAP_OR_RETURN_FALSE(contents, read_entire_file(path));

		auto type = ext_to_shader_type(path);

		if (type == GL_NONE)
		{
			fmt::println("unsupported glsl shader type found");
			continue;
		}

		auto id = glCreateShader(type);
		auto *data = contents.data();

		glShaderSource(id, 1, (const GLchar* const*)&data, nullptr);
		glCompileShader(id);

		int ok;

		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);

		if (!ok)
		{
			glGetShaderInfoLog(id, SHADER_ERROR_LOG_SIZE, nullptr, info_buffer);
			// TODO replace with an actual logger
			fmt::println("[warning] Could not compile shader at path {}. Reason:\n{}", path.c_str(), info_buffer);
			glDeleteProgram(m_program);
			return false;
		}

		glAttachShader(m_program, id);
		glDeleteShader(id);
	}

	glLinkProgram(m_program);

	int ok;

	glGetProgramiv(m_program, GL_LINK_STATUS, &ok);

	if (!ok)
	{
		glGetProgramInfoLog(m_program, SHADER_ERROR_LOG_SIZE, nullptr, info_buffer);
		// TODO replace with an actual logger
		fmt::println("[warning] Could not link shader program. Reason:\n{}", info_buffer);
		glDeleteProgram(m_program);
		return false;
	}

	return true;
}

void forge::OglShader::destroy()
{
	if (m_program > 0)
	{
		glDeleteProgram(m_program);
	}

	m_program = 0;
}

forge::OglShader::~OglShader()
{
	destroy();
}

void forge::OglShader::use() const
{
	glUseProgram(m_program);
}
