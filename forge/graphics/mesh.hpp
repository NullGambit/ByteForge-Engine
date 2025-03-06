#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace forge
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texture;
		glm::vec3 normals;
	};

	using Mesh = std::vector<Vertex>;
}
