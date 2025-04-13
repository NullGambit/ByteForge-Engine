#pragma once
#include <glm/vec3.hpp>

#define LIGHT_FIELD_COUNT 8

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

		float intensity = 1;
		float cutoff = 0.5;
		float outer_cutoff = 0.3;
		float max_distance = 15;

		bool enabled;
		bool is_available = true;

		LightType type;
	};

}
