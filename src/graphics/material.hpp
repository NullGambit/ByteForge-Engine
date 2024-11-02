#pragma once
#include <glm/vec4.hpp>

namespace forge
{
	struct Texture
	{
		std::string path;
		float scale = 1;
	};

	struct Material
	{
		glm::vec4 color {1.0};
		Texture diffuse;
	};
}
