#include "camera.hpp"

#include "core/engine.hpp"

forge::Camera::Camera(CameraProjectionMode mode, float fov, float near, float far)
{
	m_projection_mode = mode;

	m_fov = fov;
	m_near = near;
	m_far = far;

	set_projection();
}

void forge::Camera::set_mode(CameraProjectionMode mode)
{
	m_projection_mode = mode;
	set_projection();
}

void forge::Camera::set_fov(float fov)
{
	m_fov = fov;
	set_projection();
}

void forge::Camera::set_near(float near)
{
	m_near = near;
	set_projection();
}

void forge::Camera::set_far(float far)
{
	m_far = far;
	set_projection();
}

void forge::Camera::set_bounds(float near, float far)
{
	m_near = near;
	m_far = far;
	set_projection();
}

void forge::Camera::set_projection()
{
	if (m_projection_mode == CameraProjectionMode::Perspective)
	{
		auto &window = Engine::get_instance().window;

		auto win_size = window.get_size();

		m_projection = glm::perspective(glm::radians(m_fov), (float)win_size.x / win_size.y, m_near, m_far);
	}
	else
	{
		m_projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, m_near, m_far);
	}
}
