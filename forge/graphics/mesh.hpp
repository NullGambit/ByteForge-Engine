#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "material.hpp"
#include "forge/container/array.hpp"
#include "forge/container/string.hpp"
#include "forge/container/view.hpp"

namespace forge
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texture;
		glm::vec3 normals;
	};

	struct Mesh
	{
		String name;
		Array<Vertex> vertices;
		Array<u32> indices;
		Array<Material> materials;
	};

	// a trivially copyable view to a mesh
	struct MeshView
	{
		StringView name;
		View<Vertex> vertices;
		View<u32> indices;
		View<Material> materials;

		MeshView() = default;

		MeshView(Mesh &mesh);
	};
}
