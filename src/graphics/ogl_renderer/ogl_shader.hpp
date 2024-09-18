#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <mutex>

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
			std::lock_guard lock { m_mutex };
			return m_program;
		}

	private:
#ifdef SHADER_HOT_RELOAD
		uint32_t m_wd;
#endif
		mutable std::mutex m_mutex;
		ShaderSource m_source;
		uint32_t m_program;

		bool compile_implementation();
	};
}
