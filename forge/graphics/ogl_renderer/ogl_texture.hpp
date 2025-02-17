#pragma once

#include "forge/resources/resource.hpp"
#include "glad/glad.h"

namespace forge
{
	enum class TextureWrap : u16
	{
		Repeat = GL_REPEAT,
		Mirror = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
	};

	struct TextureOptions
	{
		uint32_t target = GL_TEXTURE_2D;
		TextureWrap wrap_mode = TextureWrap::Repeat;
		bool flip_on_load = false;
	};

	struct OglTexture
	{
		bool load(std::string_view path, TextureOptions options = {});

		void destroy();

		void bind(int unit = 0);

		void unbind();

		[[nodiscard]]
		u32 get_id() const
		{
			return m_id;
		}

		[[nodiscard]]
		bool is_valid() const
		{
			return m_id == UINT32_MAX;
		}

	private:
		TextureOptions m_options;
		u32 m_id = UINT32_MAX;
	};
}


