#pragma once
#include "forge/ecs/ecs.hpp"

namespace forge
{
	class CameraComponent;
}

class PlayerController : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override;

	void handle_look();

	bool is_cursor_active() const;

	void handle_other_input();

	void handle_movement(forge::DeltaTime delta);

	EXPORT_FIELDS(
		&m_speed,
		&m_boost_speed,
		&m_mouse_sensitivity);

private:
	float m_speed = 1;
	float m_boost_speed = 2;
	float m_mouse_sensitivity = 0.1;
	bool m_pan_mode = false;
	float m_pitch {};
	float m_yaw {};
	glm::vec2 m_last_mouse_coords;
	forge::CameraComponent *m_camera;

protected:
	void on_create() override;
};
