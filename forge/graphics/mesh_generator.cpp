#include "mesh_generator.hpp"

forge::Mesh forge::generate_cube()
{
	Mesh mesh;

	// mesh.reserve(24);
 //
	// // front
	// mesh.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(0, 0, -1));
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(0, 0, -1));
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(0, 0, -1));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(0, 0, -1));
 //
 //    // bottom
 //    mesh.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(0, -1, 0));
 //    mesh.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(0, -1, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(0, -1, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(0, -1, 0));
 //
 //    // back
 //    mesh.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(0, 0, 1));
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(0, 0, 1));
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(0, 0, 1));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(0, 0, 1));
 //
 //    // top
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(0, 1, 0));
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(0, 1, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(0, 1, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(0, 1, 0));
 //
 //    // left
 //    mesh.emplace_back(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0, 1), glm::vec3(-1, 0, 0));
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0, 0), glm::vec3(-1, 0, 0));
 //    mesh.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1, 1), glm::vec3(-1, 0, 0));
 //    mesh.emplace_back(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1, 0), glm::vec3(-1, 0, 0));
 //
 //    // right
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1, 1), glm::vec3(1, 0, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1, 0), glm::vec3(1, 0, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0, 1), glm::vec3(1, 0, 0));
 //    mesh.emplace_back(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(0, 0), glm::vec3(1, 0, 0));

	return mesh;
}
