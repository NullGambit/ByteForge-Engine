#include <imgui.h>
#include <iostream>
#include <numeric>
#include <ranges>

#include "config/arg_parser.hpp"
#include "container/virtual_array.hpp"
#include "glm/glm.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "core/engine.hpp"
#include "framework/components /camera_component.hpp"
#include "framework/components /register_engine_components.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"
#include "memory/arena_allocator.hpp"
#include "util/random.hpp"
#include "system/native_dialog.hpp"

class SpinCamera final : public forge::CameraComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		if (is_key_pressed(forge::Key::Space))
		{
			m_is_paused = !m_is_paused;
		}

		if (m_is_paused)
		{
			return;
		}

		auto runtime = forge::Engine::get_instance().get_engine_runtime() * speed;

		auto camera_x = glm::sin(runtime) * radius;
		auto camera_z = glm::cos(runtime) * radius;

		position = glm::vec3{camera_x, height, camera_z};
		lookat_override = glm::vec3{0.0};
	}

	EXPORT_FIELDS(
		CAMERA_COMPONENT_EXPORTED_FIELDS,
		forge::FieldSeperator{"Spin parameters"},
		&speed,
		&radius,
		&height);

	float speed = 0.5;
	float radius = 5;
	float height = 0.0f;

private:

	bool m_is_paused = false;
};

class FlyCamera final : public forge::CameraComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		handle_movement(delta);
		handle_other_input();

		if (!is_cursor_active())
		{
			handle_look();
		}
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

		yaw	 += offset.x;
		pitch += offset.y;

		pitch = glm::clamp(pitch, -90.0f, 90.0f);
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
		auto &owner = m_owner->get_entity();
		auto position = owner.get_local_position();

		auto speed = ((is_key_held(forge::Key::LeftShift) ? m_boost_speed : 0) + m_speed) * delta;

		if (is_key_held(forge::Key::W))
		{
			position += speed * get_front();
		}
		else if (is_key_held(forge::Key::S))
		{
			position -= speed * get_front();
		}
		else if (is_key_held(forge::Key::D))
		{
			position += speed * get_right();
		}
		else if (is_key_held(forge::Key::A))
		{
			position += speed * get_left();
		}

		// camera->set_position(position);
		owner.set_local_position(position);
	}

	EXPORT_FIELDS(
		CAMERA_COMPONENT_EXPORTED_FIELDS,
		forge::FieldSeperator{"Fly parameters"},
		&m_speed,
		&m_boost_speed,
		&m_mouse_sensitivity);

private:
	float m_speed = 1;
	float m_boost_speed = 2;
	float m_mouse_sensitivity = 0.1;
	bool m_pan_mode = false;
	glm::vec2 m_last_mouse_coords = forge::get_mouse_coords();
};

class ExportFieldTestComponent : public forge::IComponent
{
public:
	float f1 = 10;
	float f2 = 20;
	int integer = 500;
	bool boolean = false;
	std::string string;
	glm::vec4 vec4;
	glm::vec3 vec3;
	glm::vec2 vec2;
	glm::quat quat;
	glm::vec4 color;
	int watched_int;

	forge::ButtonField test_button =
	{
		[]
		{
			std::cout << "Button pressed\n";
		}
	};

	void on_watched_field_changed(forge::FieldVar field)
	{
		std::cout << "field changed to " << *std::get<int*>(field) << '\n';
	}

	forge::WatchedField watched_field = WATCH_FIELD(&watched_int, &ExportFieldTestComponent::on_watched_field_changed);

	EXPORT_FIELDS(
		forge::FieldSeperator{"primitives"},
		&f1, &f2, &integer, &boolean,
		forge::FieldSeperator{"controls"},
		test_button, COLOR_FIELD(&color), &watched_field,
		forge::FieldSeperator{"linear algebra"},
		&vec4, &vec3, &vec2, &quat);
};

class PrimitiveRendererComponent : public forge::IComponent
{
public:

	void on_editor_controls()
	{
	}

	~PrimitiveRendererComponent() override
	{
		m_renderer->destroy_primitive(m_data->get_id());
		m_owner->get_entity().on_transform_update.disconnect(m_on_update_connection);
	}

	void set_texture(std::string_view path, u32 type)
	{
		if (type >= forge::TextureType::Max)
		{
			return;
		}

		auto &texture = m_data->material.textures[type];

		texture.enabled = true;

		m_renderer->create_texture(m_data->get_id(), path, type);
	}

	forge::Material& get_material()
	{
		return m_data->material;
	}

	std::vector<forge::ComponentField> export_fields() override
	{
		std::vector<forge::ComponentField> out;

		for (auto type = 0; auto &texture : m_data->material.textures)
		{
			out.emplace_back(FIELD_ENTRY(forge::FieldSeperator{forge::TextureType::to_string(type)}));
			out.emplace_back<forge::ComponentField>({"Set texture", forge::ButtonField{
				[&comp = *this, type]
				{
					auto file_paths = forge::native_file_dialog({
					.root = "./assets/",
					.allow_multiple = false,
					.allowed_mimes = "image/png image/jpeg"});

					if (file_paths.empty())
					{
						return;
					}

					comp.set_texture(file_paths.front(), type);
				}
			}});
			out.emplace_back(FIELD_ENTRY(&texture.scale));
			out.emplace_back(FIELD_ENTRY(&texture.enabled));
			out.emplace_back(FIELD_ENTRY(&texture.strength));

			type++;
		}

		return out;
	}

private:
	forge::PrimitiveModel *m_data;
	forge::OglRenderer *m_renderer;
	forge::ConnectionID m_on_update_connection;

protected:
	void on_create() override
	{
		m_renderer = forge::Engine::get_instance().get_subsystem<forge::OglRenderer>();

		auto &entity = m_owner->get_entity();
		auto model = entity.get_model();

		m_data = m_renderer->create_primitive(model);

		m_on_update_connection = entity.on_transform_update.connect(
		[&data = m_data](const auto &entity)
		{
			data->update_model(entity.get_model());
		});
	}

	void on_disabled() override
	{
		m_data->is_hidden = true;
	}

	void on_enabled() override
	{
		m_data->is_hidden = false;
	}

};

class TempLightComponent : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		auto *renderer = forge::Engine::get_instance().get_subsystem<forge::OglRenderer>();

		renderer->update_light(m_owner->get_entity().get_position(), glm::vec3{m_color});
	}

	std::vector<std::type_index> get_bundle() override
	{
		return { typeid(PrimitiveRendererComponent) };
	}

	EXPORT_FIELDS(COLOR_FIELD(&m_color));

private:
	glm::vec4 m_color {1.0};
};

struct MyArgs
{
	i32 window_width;
	i32 window_height;
	std::string_view some_name = "alex";
	bool my_bool;
};

class BobComponent : public forge::IComponent
{

public:
	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_accumulated_time += delta;

		auto &owner = m_owner->get_entity();

		auto position = owner.get_local_position();

		auto angular_frequency = 2.0f * M_PI * frequency;

		auto sin_bob = amplitude * glm::sin(angular_frequency * m_accumulated_time);

		if (bob_x)
		{
			position.x = sin_bob;
		}
		if (bob_z)
		{
			position.z = sin_bob;
		}
		if (bob_y)
		{
			position.y = amplitude * glm::cos(angular_frequency * m_accumulated_time);
		}

		owner.set_local_position(position);
	}

	EXPORT_FIELDS(&frequency, &amplitude, &bob_x, &bob_y, &bob_z);

	float frequency = 0.5;
	float amplitude = 0.5;

	bool bob_x = false;
	bool bob_y = false;
	bool bob_z = true;

private:
	forge::DeltaTime m_accumulated_time {};
};

int main(int argc, const char **argv)
{
	auto &engine = forge::Engine::get_instance();

	auto result = engine.init(std::span{argv, (size_t)argc},
	{
		.window_title = "ByteForge Engine",
		.window_width = 1920,
		.window_height = 1080,
		.log_flags = log::LogTime
	});

	if (result == forge::EngineInitResult::HaltWithNoError)
	{
		return 0;
	}
	if (result != forge::EngineInitResult::Ok)
	{
		return -1;
	}

	auto *nexus = engine.get_subsystem<forge::Nexus>();

	forge::register_engine_components(nexus);

	nexus->register_component<FlyCamera>();
	nexus->register_component<ExportFieldTestComponent>();
	nexus->register_component<SpinCamera>();
	nexus->register_component<PrimitiveRendererComponent>();
	nexus->register_component<TempLightComponent>();
	nexus->register_component<BobComponent>();

	auto &player = nexus->create_entity<FlyCamera>("Player");
	//
	// auto *camera = player.get_component<FlyCamera>();
	//
	// if (camera)
	// {
	// 	camera->fov = 100;
	// }

	player.set_local_position({-5, 0, 0});

	player.emplace_child<ExportFieldTestComponent>("Child");

	auto &cube = nexus->create_entity("Cube");

	auto *primitive = cube.add_component<PrimitiveRendererComponent>();

	primitive->set_texture("assets/textures/container2.png", forge::TextureType::Diffuse);
	primitive->set_texture("assets/textures/container2_specular.png", forge::TextureType::Specular);
	primitive->set_texture("assets/textures/matrix.jpg", forge::TextureType::Emissive);

	auto &light = nexus->create_entity<TempLightComponent>("Light");

	light.set_local_position(glm::vec3{2, 1, -2});

	// constexpr auto COUNT = 10'000;
	//
	// for (auto i = 0; i < COUNT; i++)
	// {
	// 	nexus->create_entity<PrimitiveRendererComponent, BobComponent>();
	// }

	// auto *light_renderer = light.add_component<PrimitiveRendererComponent>();

	// light_renderer->set_diffuse_texture("./assets/textures/wall.jpg");

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
