#include <imgui.h>
#include <numeric>

#include "glm/glm.hpp"
#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "src/util/types.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"

class SpinCamera : public forge::IComponent
{
public:
	forge::Camera camera {forge::CameraProjectionMode::Perspective};

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		auto runtime = forge::Engine::get_instance().get_engine_runtime();
		auto radius = 10.0f;
		auto camera_x = glm::sin(runtime) * radius;
		auto camera_z = glm::cos(runtime) * radius;

		camera.look_at({camera_x, 0.0f, camera_z}, {});

		engine.renderer->update_pv(camera.pv());
	}
};

class FlyCamera : public forge::IComponent
{
public:
	forge::Camera camera {forge::CameraProjectionMode::Perspective};

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		handle_movement(delta);
		handle_other_input();

		if (!is_cursor_active())
		{
			handle_look();
		}

		auto position = camera.get_position();
		camera.look_at(position, position + camera.front_dir);
		engine.renderer->update_pv(camera.pv());
	}

	void handle_look()
	{
		auto mouse_coords = forge::get_mouse_coords();

		glm::vec2 offset
		{
			mouse_coords.x - m_last_mouse_coords.x,
			m_last_mouse_coords.y - mouse_coords.y
		};

		m_last_mouse_coords = mouse_coords;

		offset *= m_mouse_sensitivity;

		camera.yaw	 += offset.x;
		camera.pitch += offset.y;

		camera.pitch = glm::clamp(camera.pitch, -90.0f, 90.0f);

		camera.set_direction();
	}

	bool is_cursor_active() const
	{
		return forge::get_cursor_mode() == forge::CursorMode::Normal;
	}

	void handle_other_input()
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

	void handle_movement(forge::DeltaTime delta)
	{
		auto position = camera.get_position();

		auto speed = ((is_key_held(forge::Key::LeftShift) ? m_boost_speed : 0) + m_speed) * delta;

		if (is_key_held(forge::Key::W))
		{
			position += speed * camera.front_dir;
		}
		else if (is_key_held(forge::Key::S))
		{
			position -= speed * camera.front_dir;
		}
		else if (is_key_held(forge::Key::D))
		{
			position += speed * camera.get_right();
		}
		else if (is_key_held(forge::Key::A))
		{
			position += speed * camera.get_left();
		}

		camera.set_position(position);
	}

	EXPORT_FIELDS(
		m_speed,
		m_boost_speed,
		m_mouse_sensitivity);

private:
	float m_speed = 1;
	float m_boost_speed = 2;
	float m_mouse_sensitivity = 0.1;
	bool m_pan_mode = false;
	glm::vec2 m_last_mouse_coords = forge::get_mouse_coords();
};

class Test : public forge::IComponent
{
public:
	float x = 10;
	float y = 20;

	EXPORT_FIELDS(x, y);
};

int main()
{
	auto &engine = forge::Engine::get_instance();

	auto ok = engine.init(
	{
		.window_title = "ByteForge Engine",
		.window_width = 720,
		.window_height = 480,
	});

	if (!ok)
	{
		return -1;
	}

	engine.nexus->register_component<FlyCamera>(true);
	engine.nexus->register_component<Test>(true);

	auto *entity = engine.nexus->create_entity("Player");

	entity->add_components<FlyCamera>(true);

	auto &entity2 = entity->emplace_child();

	entity2.add_component<Test>();

	engine.run();

	engine.shutdown();
}
