#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>

namespace forge
{
	enum class ShaderType
	{
		Fragment,
		Vertex,
		Max,
	};

	constexpr auto SHADER_TYPE_COUNT = (size_t)ShaderType::Max;
	using ShaderSource = std::array<std::filesystem::path, SHADER_TYPE_COUNT>;

	class OglShader
	{
	public:
		bool compile(const ShaderSource &source);
		void destroy();

		OglShader() = default;

		OglShader(OglShader &&other) :
			m_program(other.m_program)
		{
			other.m_program = 0;
		}

		~OglShader();

		void use() const;

		[[nodiscard]]
		uint32_t get_program() const
		{
			return m_program;
		}

	private:
		uint32_t m_program;

		uint32_t make_program();
	};
}
