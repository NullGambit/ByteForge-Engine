#pragma once

#include "resources/resource.hpp"

namespace forge
{
	struct TextureOptions
	{
		uint32_t target;

		TextureOptions();
	};

	struct OglTexture
	{
		std::optional<uint32_t> load(std::string_view path, TextureOptions options = {});
		void destroy();

		void bind();


		TextureOptions m_options;
		uint32_t m_id;
	};
}


