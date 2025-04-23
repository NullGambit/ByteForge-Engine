#pragma once

#include <glm/fwd.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace forge
{
	struct Rotator
	{
		float pitch;
		float yaw;
		float roll;

		[[nodiscard]]
		inline glm::quat to_quat() const
		{
			return glm::quat{glm::radians(glm::vec3{pitch, yaw, roll})};
		}
	};
}
