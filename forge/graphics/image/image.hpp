#pragma once

#include <cstdint>
#include <string_view>

namespace forge
{
	struct ImageLoadOptions
	{
		bool flip = false;
		bool from_memory = false;
	};

	struct Image
	{
		int width;
		int height;
		int channels;
		uint8_t *data = nullptr;

		~Image();

		bool load(std::string_view path, ImageLoadOptions options = {});
		void unload();
	};
}
