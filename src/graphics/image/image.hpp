#pragma once

#include <cstdint>
#include <string_view>

namespace forge
{
	// TODO: make this into a resource
	struct Image
	{
		int width;
		int height;
		int channels;
		uint8_t *data = nullptr;

		Image() = default;
		Image(std::string_view path, bool load_flipped = false);
		~Image();

		bool load(std::string_view path, bool load_flipped = false);
		void unload();
	};
}
