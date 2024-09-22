#include "image.hpp"

#include "stb_image.hpp"

forge::Image::Image(std::string_view path)
{
	load(path);
}

forge::Image::~Image()
{
	unload();
}

bool forge::Image::load(std::string_view path)
{
	if (data != nullptr)
	{
		unload();
	}

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
