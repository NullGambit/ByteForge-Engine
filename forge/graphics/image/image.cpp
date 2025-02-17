#include "image.hpp"

#include "stb_image.hpp"

forge::Image::Image(std::string_view path, bool load_flipped)
{
	load(path, load_flipped);
}

forge::Image::~Image()
{
	unload();
}

bool forge::Image::load(std::string_view path, bool load_flipped)
{
	if (data != nullptr)
	{
		unload();
	}

	stbi_set_flip_vertically_on_load(load_flipped);

	data = stbi_load(path.data(), &width, &height, &channels, 0);

	return data != nullptr;
}

void forge::Image::unload()
{
	if (data)
	{
		stbi_image_free(data);
		data = nullptr;
	}
}

