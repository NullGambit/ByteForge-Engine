#include "ogl_texture.hpp"

#include "fmt/fmt.hpp"
#include "graphics/image/image.hpp"
#include "glad/glad.h"

bool forge::OglTexture::load(std::string_view path, TextureOptions options)
{
	Image image;

	if (!image.load(path, options.flip_on_load))
	{
		return false;
	}

	m_options = options;

	glGenTextures(1, &m_id);

	bind(0);

	glTexParameteri(m_options.target, GL_TEXTURE_WRAP_S, (int)m_options.wrap_mode);
	glTexParameteri(m_options.target, GL_TEXTURE_WRAP_T, (int)m_options.wrap_mode);
	glTexParameteri(m_options.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_options.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format;

	switch (image.channels)
	{
		case 4: format = GL_RGBA; break;
		case 3: format = GL_RGB;  break;
		case 2: format = GL_RG;   break;
		case 1: format = GL_RED;  break;
		default: return false;
	}

	glTexImage2D(m_options.target, 0, GL_RGB, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(m_options.target);

	return true;
}

void forge::OglTexture::destroy()
{
	if (m_id > 0)
	{
		glDeleteTextures(1, &m_id);
	}
}

void forge::OglTexture::bind(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_options.target, m_id);
}
