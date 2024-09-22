#include "ogl_texture.hpp"

#include "fmt/fmt.hpp"
#include "graphics/image/image.hpp"
#include "glad/glad.h"

forge::TextureOptions::TextureOptions()
{
	target = GL_TEXTURE_2D;
}

std::optional<uint32_t> forge::OglTexture::load(std::string_view path, TextureOptions options)
{
	Image image;

	if (!image.load(path))
	{
		return std::nullopt;
	}

	m_options = options;

	glGenTextures(1, &m_id);

	bind();

	glTexParameteri(m_options.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_options.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(m_options.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_options.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(m_options.target, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(m_options.target);

	return m_id;
}

void forge::OglTexture::destroy()
{
	if (m_id > 0)
	{
		glDeleteTextures(1, &m_id);
	}
}

void forge::OglTexture::bind()
{
	glBindTexture(m_options.target, m_id);
}
