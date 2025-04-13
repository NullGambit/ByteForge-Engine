#include "image.hpp"

#include "stb_image.hpp"

forge::Image::~Image()
{
	unload();
}

bool forge::Image::load(std::string_view path, ImageLoadOptions options)
{
	if (data != nullptr)
	{
		unload();
	}

	stbi_set_flip_vertically_on_load(options.flip);

	if (options.from_memory)
	{
		data = stbi_load_from_memory(reinterpret_cast<stbi_uc const *>(path.data()),
		path.size(), &width, &height, &channels, 0);
	}
	else
	{
		data = stbi_load(path.data(), &width, &height, &channels, 0);
	}

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

