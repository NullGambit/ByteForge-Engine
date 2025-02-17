#include "camera.hpp"

#include "forge/core/engine.hpp"

glm::mat4 forge::Camera::get_projection() const
{
	if (projection_mode == CameraProjectionMode::Perspective)
	{
		auto &window = g_engine.window;

		auto win_size = window.get_size();

		return glm::perspective(glm::radians(fov), (float)win_size.x / win_size.y, near, far);
	}

	return glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
}

