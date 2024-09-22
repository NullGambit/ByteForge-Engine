#pragma once

#include <cstdint>
#include <string_view>

namespace forge
{
	struct Image
	{
		int width;
		int height;
		int channels;
		uint8_t *data = nullptr;

		Image() = default;
		Image(std::string_view path);
		~Image();

		bool load(std::string_view path);
		void unload();
	};
}
