#include "camera_component.hpp"

#include "forge/core/engine.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

void forge::CameraComponent::on_destroy()
{
	auto *renderer = g_engine.get_subsystem<OglRenderer>();

	renderer->set_active_camera(nullptr);

	m_owner->on_transform_update.disconnect(m_on_transform_update_connection);
}

void forge::CameraComponent::on_enabled()
{
	auto *renderer = g_engine.get_subsystem<OglRenderer>();

	renderer->set_active_camera(this);
}

void forge::CameraComponent::on_create()
{
	auto *renderer = g_engine.get_subsystem<OglRenderer>();

	renderer->set_active_camera(this);

	m_on_transform_update_connection =
		m_owner->on_transform_update.connect([&position = position, &yaw = yaw, &pitch = pitch, &previous_rotation = m_previous_rotation](Entity &entity)
	{
		position = entity.get_position();
		//
		// auto current_rotation = entity.get_rotation();
		// auto delta = current_rotation * glm::inverse(previous_rotation);
		//
		// auto angle = glm::angle(delta);
		//
		// if (angle > 0.0001f) // avoid divide by zero
		// {
		// 	auto axis = glm::axis(delta);
		// 	angle = glm::degrees(angle);
		//
		// 	yaw   += angle * axis.y;
		// 	pitch += angle * axis.x;
		// }
		//
		// previous_rotation = current_rotation;
	});
}
