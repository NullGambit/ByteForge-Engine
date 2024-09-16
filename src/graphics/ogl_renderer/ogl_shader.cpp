#pragma once

#include "ogl_shader.hpp"

#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "system/io.hpp"
#include "util/macros.hpp"

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
	else
	{
		return GL_NONE;
	}
}

#define SHADER_ERROR_LOG_SIZE 512

std::optional<forge::OglShader> forge::OglShader::load(const ShaderSource &source)
{
	OglShader shader;

	shader.m_program = glCreateProgram();

	char info_buffer[SHADER_ERROR_LOG_SIZE];

	for (auto &path : source)
	{
		if (path.empty())
		{
			continue;
		}

		UNWRAP_OR_RETURN(contents, read_entire_file(path));

		auto type = ext_to_shader_type(path);
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
			return std::nullopt;
		}

		glAttachShader(shader.m_program, id);
		// glDeleteShader(id);
	}

	glLinkProgram(shader.m_program);

	int ok;

	glGetProgramiv(shader.m_program, GL_LINK_STATUS, &ok);

	if (!ok)
	{
		glGetProgramInfoLog(shader.m_program, SHADER_ERROR_LOG_SIZE, nullptr, info_buffer);
		// TODO replace with an actual logger
		fmt::println("[warning] Could not link shader program. Reason:\n{}", info_buffer);
		return std::nullopt;
	}

	return shader;
}

forge::OglShader::~OglShader()
{
	if (m_program > 0)
	{
		glDeleteProgram(m_program);
	}
}

void forge::OglShader::use()
{
	glUseProgram(m_program);
}
