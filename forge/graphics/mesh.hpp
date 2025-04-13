#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "material.hpp"
#include "forge/container/array.hpp"
#include "forge/container/string.hpp"

namespace forge
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texture;
	};

	struct Mesh
	{
		String name;
		Array<Vertex> vertices;
		Array<u32> indices;
		Array<Material> materials;
	};
}
