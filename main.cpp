#include <imgui.h>
#include <iostream>
#include <numeric>

#include "glm/glm.hpp"
#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "src/util/types.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"

class SpinCamera final : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	forge::Camera camera {forge::CameraProjectionMode::Perspective};

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		auto runtime = forge::Engine::get_instance().get_engine_runtime() * m_speed;
		auto camera_x = glm::sin(runtime) * m_radius;
		auto camera_z = glm::cos(runtime) * m_radius;

		camera.look_at({camera_x, m_height, camera_z}, {});

		engine.renderer->update_pv(camera.pv());
	}

	EXPORT_FIELDS(m_speed, m_radius, m_height);

private:
	float m_speed = 0.5;
	float m_radius = 10;
	float m_height = 0.0f;
};

class FlyCamera final : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

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
		auto &position = m_owner.get().get_transform().m_position;

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

class PrimitiveRendererComponent : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_renderer->update_primitive(m_id, m_owner.get().get_transform().m_model);
	}

	void on_editor_controls()
	{
		ImGui::Text("render id: %d", m_id);
	}

private:
	u32 m_id;
	forge::OglRenderer *m_renderer;

protected:
	void on_enter() override
	{
		m_renderer = forge::Engine::get_instance().renderer;

		auto &transform = m_owner.get().get_transform();

		auto model = transform.compute_local_transform();

		m_id = m_renderer->create_primitive(model);
	}
};

int main()
{
	auto &engine = forge::Engine::get_instance();

	auto ok = engine.init(
	{
		.window_title = "ByteForge Engine",
		.window_width = 1920,
		.window_height = 1080,
	});

	if (!ok)
	{
		return -1;
	}

	engine.nexus->register_component<FlyCamera>();
	engine.nexus->register_component<Test>();
	engine.nexus->register_component<SpinCamera>();
	engine.nexus->register_component<PrimitiveRendererComponent>();

	auto &entity = engine.nexus->create_entity("Player");
	entity.add_component<FlyCamera>();

	entity.get_transform().m_position = {0, 0, 5};

	engine.nexus->add_to_group("important entities", entity);

	// auto &entity2 = entity.emplace_child("Child");

	// entity2.add_component<Test>();

	engine.nexus->create_entity<PrimitiveRendererComponent>("David john smith");

	auto &john = engine.nexus->create_entity("john allen");

	engine.nexus->add_to_group("important entities", john);

	engine.nexus->destroy_entity(&john);

	engine.nexus->create_entity("johnny");
	engine.nexus->create_entity("D");

	auto player = engine.nexus->get_entity("Player");

	if (!player.has_value())
	{
		log::info("could not find player");
		return -1;
	}

	for (auto &child : player.get().get_children())
	{
		log::info("{}", child.get_name());
	}

	for (auto &entity_view : *engine.nexus->get_group("important entities"))
	{
		if (entity_view.is_entity_valid())
		{
			log::info("{}", entity_view.get().get_name());
		}
	}

	engine.run();

	engine.shutdown();
}
