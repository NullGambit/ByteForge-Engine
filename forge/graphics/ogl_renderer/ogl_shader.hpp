#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <variant>

#include "forge/container/map.hpp"
#include "forge/core/forge_intrinsic.hpp"
#include "glm/glm.hpp"

#define SHOULD_USE_UNIFORM_CACHE defined(SHADER_HOT_RELOAD) || defined(FORGE_USE_UNIFORM_CACHE)
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
	using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

	class OglShader
	{
	public:
		bool compile(const ShaderSource &source);
		void destroy();

		~OglShader();

		OglShader& use();

		[[nodiscard]]
		uint32_t get_program() const
		{
			return m_program;
		}

        OglShader& set(std::string_view name, int value);
        OglShader& set(std::string_view name, float value);
        OglShader& set(std::string_view name, glm::vec2 value);
        OglShader& set(std::string_view name, glm::vec3 value);
        OglShader& set(std::string_view name, glm::vec4 value);
        OglShader& set(std::string_view name, glm::mat4 value);

	private:
#ifdef FORGE_SHADER_HOT_RELOAD
		u32 m_wd;
		// a cache for uniforms used to set uniforms to their previous state when shaders reload
		ShaderSource m_source;
#endif

#if SHOULD_USE_UNIFORM_CACHE
		HashMap<std::string, UniformValue, ENABLE_TRANSPARENT_HASH> m_cache;
#endif

		uint32_t m_program;

		bool compile_implementation(const ShaderSource &source);

#if SHOULD_USE_UNIFORM_CACHE
		template<class T>
		bool update_cache(std::string_view name, T &value)
		{
			auto iter = m_cache.find(name);

			if (iter != m_cache.end())
			{
				if (std::get<T>(iter->second) == value)
				{
					return true;
				}
			}

			m_cache[name] = value;

			return false;
		}
#endif

	};
}
