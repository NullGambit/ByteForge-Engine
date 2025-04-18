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
		m_owner->on_transform_update.connect([&position = position, &yaw = yaw, &pitch = pitch](Entity &entity)
	{
		position = entity.get_local_position();
		// auto rotation = entity.get_local_euler_rotation();
		// yaw = rotation.y;
		// pitch = rotation.x;
	});
}
