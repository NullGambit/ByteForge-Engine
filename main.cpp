#include <imgui.h>
#include <iostream>
#include <numeric>

#include "glm/glm.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "src/util/types.hpp"
#include "core/engine.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"
#include "memory/arena_allocator.hpp"
#include "util/random.hpp"

class SpinCamera final : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	forge::Camera camera {forge::CameraProjectionMode::Perspective};

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		auto runtime = forge::Engine::get_instance().get_engine_runtime() * speed;

		auto camera_x = glm::sin(runtime) * radius;
		auto camera_z = glm::cos(runtime) * radius;

		camera.look_at({camera_x, height, camera_z}, {});

		engine.renderer->update_pv(camera.pv());
	}

	EXPORT_FIELDS(speed, radius, height);

	float speed = 0.5;
	float radius = 10;
	float height = 0.0f;
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
		auto &transform = m_owner->get_entity().get_transform();
		auto position = transform.get_local_position();

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
		transform.set_local_position(position);
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
	float f1 = 10;
	float f2 = 20;
	int integer = 500;
	std::string string;
	forge::ButtonField test_button
	{
		[]
		{
			log::info("button pressed");
		}
	};

	EXPORT_FIELDS(f1, f2, integer, test_button);
};

class PrimitiveRendererComponent : public forge::IComponent
{
public:

	void on_editor_controls()
	{
		ImGui::Text("render id: %d", m_id);
	}

	~PrimitiveRendererComponent() override
	{
		m_renderer->destroy_primitive(m_id);
		m_owner->get_entity().on_entity_transform_updated.disconnect(m_on_update_connection);
	}

private:
	u32 m_id;
	forge::OglRenderer *m_renderer;
	forge::ConnectionID m_on_update_connection;

protected:
	void on_enter() override
	{
		m_renderer = forge::Engine::get_instance().renderer;

		auto &entity = m_owner->get_entity();
		auto model = entity.get_model();

		m_id = m_renderer->create_primitive(model);

		m_on_update_connection = entity.on_entity_transform_updated.connect(
		[&renderer = m_renderer, &id = m_id](const forge::Transform &transform)
		{
			renderer->update_primitive(id, transform.get_model());
		});
	}

	void on_disabled() override
	{
		m_renderer->primitive_set_hidden(m_id, true);
	}

	void on_enabled() override
	{
		m_renderer->primitive_set_hidden(m_id, false);
	}
};

class CounterComponent : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_counter++;
	}

private:
	u64 m_counter = 0;
};

class ClusteredRenderingComponent : public forge::IComponent
{
public:

	struct ClusterEntry
	{
		u32 id;
		glm::mat4 model;
	};

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		if (m_last_mesh_count != m_mesh_count)
		{
			if (m_last_mesh_count < m_mesh_count)
			{
				add_meshes(m_mesh_count - m_last_mesh_count);
			}
		}

		m_last_mesh_count = m_mesh_count;
	}

	~ClusteredRenderingComponent() override
	{
		for (auto &entry : m_meshes)
		{
			m_renderer->destroy_primitive(entry.id);
		}
	}

	EXPORT_FIELDS(m_mesh_count);

private:
	std::vector<ClusterEntry> m_meshes;
	int m_mesh_count = 1;
	int m_last_mesh_count = m_mesh_count;
	forge::OglRenderer *m_renderer;

protected:
	void on_enter() override
	{
		m_renderer = forge::Engine::get_instance().renderer;

		add_meshes(m_mesh_count);
	}

	void on_disabled() override
	{
		set_all_hidden(true);
	}

	void on_enabled() override
	{
		set_all_hidden(false);
	}

private:

	void set_all_hidden(bool value)
	{
		for (auto &entry : m_meshes)
		{
			m_renderer->primitive_set_hidden(entry.id, value);
		}
	}

	void add_meshes(int count)
	{
		glm::mat4 model {1.0};

		for (auto i = 0; i < count; i++)
		{
			auto primitive_model = glm::translate(model, util::rand_vec3(-10, 10));

			primitive_model = glm::scale(primitive_model, glm::vec3{util::rand_float(0.1, 2)});

			primitive_model = glm::rotate(primitive_model, util::rand_float(0, 1), normalize(util::rand_vec3(-360, 360)));

			auto id = m_renderer->create_primitive(primitive_model);

			m_meshes.emplace_back(id, model);
		}
	}
};

class BusyWorkComponent : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_result *= glm::mat4{5.0};
	}

private:
	glm::mat4 m_result {1.0};
};

class BusyWorkComponent2 : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_result *= glm::mat4{5.0};
	}

private:
	glm::mat4 m_result {1.0};
};

class BusyWorkComponent3 : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_result *= glm::mat4{5.0};
	}

private:
	glm::mat4 m_result {1.0};
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
	engine.nexus->register_component<ClusteredRenderingComponent>();

	auto &entity = engine.nexus->create_entity("Player");

	auto *spin_camera = entity.add_component<SpinCamera>();

	spin_camera->radius = 30;

	entity.get_transform().set_local_position({0, 0, 5});

	engine.nexus->add_to_group("important entities", entity);

	entity.emplace_child<Test>("Child");

	engine.nexus->create_entity<ClusteredRenderingComponent>("David john smith");

	auto &john = engine.nexus->create_entity("john allen");

	engine.nexus->add_to_group("important entities", john);

	engine.nexus->destroy_entity(&john);

	engine.nexus->create_entity("johnny");
	engine.nexus->create_entity("D");

	auto player = engine.nexus->get_entity("Player");

	if (!player->has_value())
	{
		log::info("could not find player");
		return -1;
	}

	// constexpr auto ENTITY_COUNT = 400'000;
	//
	// for (auto i = 0; i < ENTITY_COUNT; i++)
	// {
	// 	auto name = "Entity_" + std::to_string(i);
	// 	engine.nexus->create_entity<CounterComponent, BusyWorkComponent, BusyWorkComponent2, BusyWorkComponent3>(name);
	// }

	engine.run();

	engine.shutdown();
}
