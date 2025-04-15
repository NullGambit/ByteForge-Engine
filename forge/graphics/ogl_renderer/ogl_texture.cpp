#include "ogl_texture.hpp"

#include "forge/core/logging.hpp"
#include "forge/graphics/image/image.hpp"
#include "glad/glad.h"

bool forge::OglTexture::load(const Image &image, TextureOptions options)
{
	target = options.target;

	glGenTextures(1, &id);

	bind(0);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, (int)options.wrap_mode);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, (int)options.wrap_mode);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format;

	switch (image.channels)
	{
		case 4: format = GL_RGBA; break;
		case 3: format = GL_RGB;  break;
		case 2: format = GL_RG;   break;
		case 1: format = GL_RED;  break;
		default: return false;
	}

	glTexImage2D(options.target, 0, GL_RGBA, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(options.target);

	return true;
}

bool forge::OglTexture::load(std::string_view path, TextureOptions options)
{
	Image image;

	if (!image.load(path, options.image_options))
	{
		return false;
	}

	return load(image, options);
}

void forge::OglTexture::destroy()
{
	if (id > 0)
	{
		glDeleteTextures(1, &id);
		id = 0;
	}
}

void forge::OglTexture::bind(int unit) const
{
	if (is_valid())
	{
		return;
	}

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, id);
}

void forge::OglTexture::unbind()
{
	glBindTexture(target, 0);
}
