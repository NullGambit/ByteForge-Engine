#include "ogl_shader.hpp"

#include "ogl_renderer.hpp"
#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "core/engine.hpp"
#include "core/logging.hpp"
#include "system/io.hpp"
#include "util/macros.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "util/types.hpp"

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

void set_uniform(uint32_t program, std::string_view name, const forge::UniformValue &value)
{
	auto location = glGetUniformLocation(program, name.data());

	std::visit(util::overload
	{
		[location](int value)
		{
			glUniform1i(location, value);
		},
		[location](float value)
		{
			glUniform1f(location, value);
		},
		[location](glm::vec2 value)
		{
			glUniform2f(location, EXPAND_VEC2(value));
		},
		[location](glm::vec3 value)
		{
			glUniform3f(location, EXPAND_VEC3(value));
		},
		[location](glm::vec4 value)
		{
			glUniform4f(location, EXPAND_VEC4(value));
		},
		[location](glm::mat4 value)
		{
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
		},
	}, value);
}

bool forge::OglShader::compile(const ShaderSource &source)
{
	m_source = source;

	auto &engine = Engine::get_instance();

	auto ok = compile_implementation();

#ifdef SHADER_HOT_RELOAD
	if (ok)
	{
		for (auto &path : m_source)
		{
			if (path.empty())
			{
				continue;
			}

			m_wd = engine.fs_monitor->add_watch(path.c_str(), FSE_MODIFY, [&](auto events, auto p)
			{
				auto &engine = Engine::get_instance();

				engine.renderer->add_command([&]
				{
					destroy();
					compile_implementation();

					glUseProgram(m_program);

					for (auto &[name, value] : m_cache)
					{
						set_uniform(m_program, name, value);
					}

					log::info("reloaded shader at path {}", path.c_str());
				});
			});
		}
	}
#endif

	return ok;
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

#ifdef SHADER_HOT_RELOAD
	auto &engine = Engine::get_instance();
	engine.fs_monitor->remove_watch(m_wd);
#endif

	destroy();
}

forge::OglShader& forge::OglShader::use()
{
	glUseProgram(m_program);
	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, int value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniform1i(location, value);

#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif

	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, float value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniform1f(location, value);
#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif
	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, glm::vec2 value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniform2f(location, EXPAND_VEC2(value));
#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif
	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, glm::vec3 value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniform3f(location, EXPAND_VEC3(value));
#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif
	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, glm::vec4 value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniform4f(location, EXPAND_VEC4(value));
#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif
	return *this;
}

forge::OglShader& forge::OglShader::set(std::string_view name, glm::mat4 value)
{
	auto location = glGetUniformLocation(m_program, name.data());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
#ifdef SHADER_HOT_RELOAD
	m_cache[name] = value;
#endif
	return *this;
}

struct ShaderParser
{
	const std::string &source;
	size_t off = 0;

	ShaderParser(const std::string &source) :
		source(source)
	{}

	bool at_end()
	{
		return off >= source.size();
	}

	char peak()
	{
		if (at_end())
		{
			return '\0';
		}

		return source[off];
	}

	char peak_next()
	{
		if (off+1 >= source.size())
		{
			return '\0';
		}

		return source[off + 1];
	}

	char advance()
	{
		return source[off++];
	}

	bool match(char c)
	{
		if (peak() == c)
		{
			++off;
			return true;
		}

		return false;
	}

	bool match_two(char a, char b)
	{
		if (peak() == a && peak_next() == b)
		{
			off += 2;
			return true;
		}

		return false;
	}

	void scan_alpha()
	{
		while (!at_end() && isalpha(peak()))
		{
			advance();
		}
	}
};

// a terrible parser written to preprocess headers
std::optional<std::string> preprocess_shader(const std::string &source)
{
	ShaderParser parser {source};
	std::string out;

	out.reserve(source.size() * 2);

	while (!parser.at_end())
	{
		if (parser.match_two('/', '/'))
		{
			while (!parser.at_end() && !parser.match('\n'))
			{
				parser.advance();
			}
		}
		else if (parser.match_two('/', '*'))
		{
			while (!parser.at_end() && !parser.match_two('/', '*'))
			{
				parser.advance();
			}
		}
		else if (parser.match('#'))
		{
			size_t start = parser.off;

			parser.scan_alpha();

			std::string_view name {source.data() + start, parser.off - start};

			if (name != "include")
			{
				out += "#";
				out += name;

				continue;
			}

			fmt::println(name);

			parser.advance();

			start = parser.off;

			parser.scan_alpha();

			std::string_view path {source.data() + start, parser.off - start};

			parser.advance();

			UNWRAP_OR_RETURN(contents, forge::read_entire_file(path));

			out += contents;
		}
		else
		{
			out += parser.advance();
		}
	}

	return out;
}

bool forge::OglShader::compile_implementation()
{
	m_program = glCreateProgram();

	char info_buffer[SHADER_ERROR_LOG_SIZE];

	for (auto &path : m_source)
	{
		if (path.empty())
		{
			continue;
		}

		UNWRAP_OR_RETURN_FALSE(contents, read_entire_file(path));

		auto type = ext_to_shader_type(path);

		if (type == GL_NONE)
		{
			log::warn("unsupported glsl shader type found");
			continue;
		}

		// UNWRAP_OR_RETURN_FALSE(processed_contents, preprocess_shader(contents));

		auto id = glCreateShader(type);
		auto *data = contents.data();

		glShaderSource(id, 1, (const GLchar* const*)&data, nullptr);
		glCompileShader(id);

		int ok;

		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);

		if (!ok)
		{
			glGetShaderInfoLog(id, SHADER_ERROR_LOG_SIZE, nullptr, info_buffer);

			log::warn("Could not compile shader at path {}. Reason:\n{}", path.c_str(), info_buffer);

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

		log::warn("Could not link shader program. Reason:\n{}", info_buffer);

		glDeleteProgram(m_program);

		return false;
	}

	return true;
}
