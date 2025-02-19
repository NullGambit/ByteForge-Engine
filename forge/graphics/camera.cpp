#include "camera.hpp"

#include "forge/system/window_sub_system.hpp"

glm::mat4 forge::Camera::get_projection() const
{
	if (projection_mode == CameraProjectionMode::Perspective)
	{
		auto win_size = g_main_window->get_size();

		return glm::perspective(glm::radians(fov), (float)win_size.x / win_size.y, near, far);
	}

	return glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
}

