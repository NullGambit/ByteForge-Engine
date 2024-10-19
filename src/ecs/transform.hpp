#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

namespace forge
{
	struct Transform
	{
		glm::vec3 position {1.0};
		glm::vec3 scale {1.0};
		glm::vec3 euler;
		glm::mat4 model {1.0};

		[[nodiscard]]
		glm::mat4 get_local_transform() const
		{
			const auto transform_x = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
			const auto transform_y = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
			const auto transform_z = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0.0f, 0.0f, 1.0f));
			const auto rotation_matrix = transform_y * transform_x * transform_z;

			return glm::translate(glm::mat4(1.0f), position) * rotation_matrix * glm::scale(glm::mat4(1.0f), scale);
		}
	};
}
