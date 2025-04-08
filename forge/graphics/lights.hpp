#pragma once
#include <glm/vec3.hpp>

namespace forge
{
	enum class LightType : u8
	{
		Direction,
		Spot,
		Point,
	};

	struct Light
	{
		glm::vec3  position;
		glm::vec3  direction;
		glm::vec3  color {1};

		float cutoff;
		float outer_cutoff;

		float linear;
		float quadratic;
		bool enabled;
		bool is_available = true;
		LightType type;
	};
}
