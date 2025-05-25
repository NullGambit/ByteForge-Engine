#pragma once
#include "forge/ecs/ecs.hpp"

namespace forge
{
	class CameraComponent;
}

class PlayerController : public forge::IComponent
{
public:

	float speed = 1;
	float acceleration = 1;
	float boost_speed = 2;
	float mouse_sensitivity = 0.1;
	bool use_camera_direction = false;

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override;

	void handle_look();

	bool is_cursor_active() const;

	void handle_other_input();

	void handle_movement(forge::DeltaTime delta);

	EXPORT_FIELDS(
		&use_camera_direction,
		&speed,
		&acceleration,
		&boost_speed,
		&mouse_sensitivity);

private:
	glm::vec2 m_last_mouse_coords {};
	forge::CameraComponent *m_camera = nullptr;

protected:
	void on_begin() override;
};
