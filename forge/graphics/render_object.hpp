#pragma once

#include "material.hpp"
#include "forge/ecs/transform.hpp"

namespace forge
{
	struct RenderObject
	{
		u32 flags;
		Material material;
		glm::mat4 normal_matrix;
		glm::mat4 model;
		u32 id;

		void compute_model(const glm::mat4 &new_model)
		{
			model = new_model;
			normal_matrix = glm::transpose(glm::inverse(model));
		}
	};
}
