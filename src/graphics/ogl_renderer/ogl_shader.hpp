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

	constexpr auto ShaderTypeCount = (size_t)ShaderType::Max;
	using ShaderSource = std::array<std::filesystem::path, ShaderTypeCount>;

	class OglShader
	{
	public:
		static std::optional<OglShader> load(const ShaderSource &source);

		~OglShader();

		void use();

	private:
		uint32_t m_program;

		uint32_t make_program();

	};
}
