#include "mesh_generator.hpp"

namespace forge
{
	static Mesh g_cube_cache;

	Mesh generate_cube_impl()
	{
		Mesh mesh;

		mesh.vertices.reserve(24);

		// front
		mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(0, 0, -1));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(0, 0, -1));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(0, 0, -1));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(0, 0, -1));

	    // bottom
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(0, -1, 0));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(0, -1, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(0, -1, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(0, -1, 0));

	    // back
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(0, 0, 1));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(0, 0, 1));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(0, 0, 1));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(0, 0, 1));

	    // top
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(0, 1, 0));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(0, 1, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(0, 1, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(0, 1, 0));

	    // left
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(-1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(-1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(-1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(-1, 0, 0));

	    // right
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(1, 0, 0));
	    mesh.vertices.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(1, 0, 0));

		mesh.indices.reserve(36);

		mesh.indices.emplace_back(0); mesh.indices.emplace_back(1); mesh.indices.emplace_back(2);
		mesh.indices.emplace_back(2); mesh.indices.emplace_back(1); mesh.indices.emplace_back(3);

		// bottom
		mesh.indices.emplace_back(4); mesh.indices.emplace_back(5); mesh.indices.emplace_back(6);
		mesh.indices.emplace_back(6); mesh.indices.emplace_back(5); mesh.indices.emplace_back(7);

		// back
		mesh.indices.emplace_back(10); mesh.indices.emplace_back(9); mesh.indices.emplace_back(8);
		mesh.indices.emplace_back(11); mesh.indices.emplace_back(9); mesh.indices.emplace_back(10);

		// top
		mesh.indices.emplace_back(14); mesh.indices.emplace_back(13); mesh.indices.emplace_back(12);
		mesh.indices.emplace_back(15); mesh.indices.emplace_back(13); mesh.indices.emplace_back(14);

		// left
		mesh.indices.emplace_back(16); mesh.indices.emplace_back(17); mesh.indices.emplace_back(18);
		mesh.indices.emplace_back(18); mesh.indices.emplace_back(17); mesh.indices.emplace_back(19);

		// right
		mesh.indices.emplace_back(22); mesh.indices.emplace_back(21); mesh.indices.emplace_back(20);
		mesh.indices.emplace_back(23); mesh.indices.emplace_back(21); mesh.indices.emplace_back(22);

		return mesh;
	}
}

forge::MeshView forge::generate_cube()
{
	if (g_cube_cache.vertices.empty())
	{
		g_cube_cache = generate_cube_impl();
	}

	return g_cube_cache;
}
