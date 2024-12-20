#include "camera_component.hpp"

#include "core/engine.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"

void forge::CameraComponent::on_destroy()
{
	auto *renderer = Engine::get_instance().get_subsystem<OglRenderer>();

	renderer->set_active_camera(nullptr);

	auto &owner = m_owner->get_entity();

	owner.on_transform_update.disconnect(m_on_transform_update_connection);
}

void forge::CameraComponent::on_enabled()
{
	auto *renderer = Engine::get_instance().get_subsystem<OglRenderer>();

	renderer->set_active_camera(this);
}

void forge::CameraComponent::on_create()
{
	auto *renderer = Engine::get_instance().get_subsystem<OglRenderer>();

	renderer->set_active_camera(this);

	auto &owner = m_owner->get_entity();

	m_on_transform_update_connection =
		owner.on_transform_update.connect([&position = position](auto &entity)
	{
		position = entity.get_local_position();
	});
}
