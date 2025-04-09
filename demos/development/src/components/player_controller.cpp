#include "player_controller.hpp"

#include "forge/framework/input.hpp"
#include "forge/framework/components/camera_component.hpp"

void PlayerController::update(forge::DeltaTime delta)
{
	handle_movement(delta);
	handle_other_input();

	if (!is_cursor_active())
	{
		handle_look();
	}
}

void PlayerController::handle_look()
{
	auto mouse_coords = forge::get_mouse_coords();

	glm::vec2 offset
	{
		mouse_coords.x - m_last_mouse_coords.x,
		m_last_mouse_coords.y - mouse_coords.y
	};

	m_last_mouse_coords = mouse_coords;

	offset *= m_mouse_sensitivity;

	m_camera->yaw	+= offset.x;
	m_camera->pitch += offset.y;

	m_camera->pitch = glm::clamp(m_camera->pitch, -90.0f, 90.0f);

	auto &owner = m_owner->get_entity();
	auto rotation = owner.get_euler_rotation();

	rotation.y = -m_camera->yaw;
	rotation.z = m_camera->pitch;

	owner.set_local_rotation(rotation);
}

bool PlayerController::is_cursor_active() const
{
	return forge::get_cursor_mode() == forge::CursorMode::Normal;
}

void PlayerController::handle_other_input()
{
	using enum forge::CursorMode;

	if (is_key_pressed(forge::Key::F1))
	{
		auto new_mode = is_cursor_active() ? Disabled : Normal;
		set_cursor_mode(new_mode);
	}
	else if (is_key_pressed(forge::Key::F2))
	{
		m_pan_mode = !m_pan_mode;
	}

	if (m_pan_mode)
	{
		auto should_disable = is_mouse_button_held(forge::MouseButton::Right);
		set_cursor_mode(should_disable ? Disabled : Normal);
	}
}

void PlayerController::handle_movement(forge::DeltaTime delta)

{
	auto &owner = m_owner->get_entity();
	auto position = owner.get_local_position();

	auto speed = ((is_key_held(forge::Key::LeftShift) ? m_boost_speed : 0) + m_speed) * delta;

	if (is_key_held(forge::Key::W))
	{
		position += speed * m_camera->get_front();
	}
	if (is_key_held(forge::Key::S))
	{
		position -= speed * m_camera->get_front();
	}
	if (is_key_held(forge::Key::D))
	{
		position += speed * m_camera->get_right();
	}
	if (is_key_held(forge::Key::A))
	{
		position += speed * m_camera->get_left();
	}
	if (is_key_held(forge::Key::Space))
	{
		position.y += speed;
	}
	if (is_key_held(forge::Key::LeftControl))
	{
		position.y -= speed;
	}

	owner.set_local_position(position);
}

void PlayerController::on_create()
{
	m_camera = m_owner->get_entity().get_component<forge::CameraComponent>();
}
