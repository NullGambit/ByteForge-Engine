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
	offset *= mouse_sensitivity;

	m_camera->yaw += offset.x;
	m_camera->pitch += offset.y;

	m_camera->pitch = glm::clamp(m_camera->pitch, -89.0f, 89.0f);

	m_owner->rotate(-m_camera->yaw, 0, 0);
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
}

float bool_input(bool positive, bool negative)
{
	if (positive)
	{
		return 1;
	}
	if (negative)
	{
		return -1;
	}
	return 0;
}

glm::vec3 get_input_vector()
{
	return glm::vec3
	{
		bool_input(is_key_held(forge::Key::W), is_key_held(forge::Key::S)),
		bool_input(is_key_held(forge::Key::Space), is_key_held(forge::Key::LeftControl)),
		bool_input(is_key_held(forge::Key::D), is_key_held(forge::Key::A)),
	};
}

void PlayerController::handle_movement(forge::DeltaTime delta)
{
	auto position = m_owner->get_local_position();

	auto dir_multiplier = ((is_key_held(forge::Key::LeftShift) ? boost_speed : 0) + speed) * delta;

	auto input = get_input_vector();

	// if basis * input is normalized and input is zero will result in a nan vector
	if (glm::zero<glm::vec3>() == input)
	{
		return;
	}

	const auto basis = use_camera_direction ? m_camera->get_basis_mat() : m_owner->get_basis_mat();
	const auto direction = glm::normalize(basis * input);

	position += dir_multiplier * direction;

	const auto target_pos = position + direction * dir_multiplier;
	const auto weight = acceleration;

	position = glm::mix(position, target_pos, weight);

	m_owner->set_local_position(position);
}

void PlayerController::on_begin()
{
	m_camera = m_owner->get_component<forge::CameraComponent>();

	assert(m_camera && "Player controller must have a camera");
}
