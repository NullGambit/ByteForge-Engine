#pragma once

#include "resources/resource.hpp"
#include "glad/glad.h"

namespace forge
{
	enum class TextureWrap
	{
		Repeat = GL_REPEAT,
		Mirror = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
	};

	struct TextureOptions
	{
		uint32_t target = GL_TEXTURE_2D;
		bool flip_on_load = false;
		TextureWrap wrap_mode = TextureWrap::Repeat;
	};

	struct OglTexture
	{
		bool load(std::string_view path, TextureOptions options = {});
		void destroy();

		void bind(int unit = 0);

		uint32_t get_id() const
		{
			return m_id;
		}

	private:
		TextureOptions m_options;
		uint32_t m_id;
	};
}


