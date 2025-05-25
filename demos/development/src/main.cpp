
#include <thread>
#include <unordered_set>
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>

#include "components/bob_component.hpp"
#include "components/export_test_component.hpp"
#include "components/lifetime_component.hpp"
#include "../../../forge/framework/components/light_component.hpp"
#include "../../../forge/framework/components/mesh_renderer_component.hpp"
#include "components/player_controller.hpp"
#include "forge/container/array.hpp"
#include "forge/container/set.hpp"
#include "forge/editor/editor_subsystem.hpp"
#include "forge/events/timer.hpp"
#include "forge/framework/mesh_loading.hpp"
#include "forge/framework/components/camera_component.hpp"
#include "forge/framework/components/register_engine_components.hpp"
#include "forge/util/random.hpp"

void register_demo_components()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->register_component<PlayerController>();
	nexus->register_component<BobComponent>();
	nexus->register_component<ExportFieldTestComponent>();
	nexus->register_component<LifetimeComponent>();
}

void export_test_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->create_entity<ExportFieldTestComponent>();
}

void make_player()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	auto *player_ent = nexus->create_entity<PlayerController, forge::CameraComponent>("player");
}

void diseappring_cube_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	auto *cube_entity = nexus->create_entity("cube");

	auto *cube = cube_entity->add_component<forge::MeshRendererComponent>();

	cube->set_mesh(forge::PrimitiveMeshType::Cube);

	cube->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);

	auto cube_position = cube_entity->get_local_position();

	cube_position.x = 3;
	cube_position.y = -1;
	cube_position.z = -2;

	cube_entity->set_local_position(cube_position);

	auto lifetime = cube_entity->add_component<LifetimeComponent>();

	lifetime->duration = forge::Seconds{10} + forge::Hours{1};

	lifetime->start();
}

template <typename T> int get_sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

void crates_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	auto *ground_ent = nexus->create_entity("ground");

	auto *ground_mesh = ground_ent->add_component<forge::MeshRendererComponent>();

	ground_mesh->set_mesh(forge::PrimitiveMeshType::Cube);

	ground_mesh->set_texture(DEMO_ASSET_DIR"textures/concrete.jpg", forge::TextureType::Diffuse);

	ground_mesh->object->material.textures[forge::TextureType::Specular].strength = 2;
	ground_mesh->object->material.textures[forge::TextureType::Diffuse].scale = 5;

	auto ground_pos = ground_ent->get_local_position();

	ground_pos.x = 2;
	ground_pos.y = -1.5;
	ground_pos.z = 0;

	ground_ent->set_local_position(ground_pos);

	auto ground_scale = ground_ent->get_scale();

	ground_scale.x = 20;
	ground_scale.y = 0.5;
	ground_scale.z = 20;

	ground_ent->set_scale(ground_scale);

	auto *light_ent = nexus->create_entity("light");

	auto *light = light_ent->add_component<forge::LightComponent>()->get_light();

	light->type = forge::LightType::Direction;

	light_ent->set_local_rotation({-0.2f, -1.0f, -0.3f});

	forge::Array<glm::vec3> positions;

	srand(1);

	for (int i = 0; i < 15; i++)
	{
		auto *entity = nexus->create_entity(fmt::format("crate_{}", i));
		auto *crate = entity->add_component<forge::MeshRendererComponent>();

		crate->set_mesh(forge::PrimitiveMeshType::Cube);

		crate->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);
		crate->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2_specular.png", forge::TextureType::Specular);

		auto position = util::rand_vec3(-8, 8);

		position.y = -0.75;

		auto length = glm::length(position);

		for (const auto &other_pos : positions)
		{
			auto diff = std::abs(glm::length(other_pos) - length);

			if (diff <= 5)
			{
				auto x_sign = -get_sign(other_pos.x);
				auto z_sign = -get_sign(other_pos.z);

				position.x *= x_sign;
				position.z *= z_sign;
			}
		}

		auto rotation = util::rand_vec3(-360, 360);
		rotation.x = 0;
		rotation.z = 0;

		entity->set_local_position(position);
		entity->set_local_rotation(rotation);

		positions.emplace_back(position);
	}
}

void bobbing_cluster()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	constexpr auto COUNT = 100;
	constexpr auto DISTANCE = 25;

	for (auto i = 0; i < COUNT; i++)
	{
		auto *cube_entity = nexus->create_entity(fmt::format("cube_{}", i));

		auto *cube = cube_entity->add_component<forge::MeshRendererComponent>();
		cube->set_mesh(forge::PrimitiveMeshType::Cube);
		auto *bob = cube_entity->add_component<BobComponent>();

		bob->amplitude = util::rand_float(0.1, 2);
		bob->amplitude = util::rand_float(0.1, 2);

		cube->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);

		cube_entity->set_local_position(util::rand_vec3(-DISTANCE, DISTANCE));

		cube_entity->set_scale(glm::vec3{util::rand_float(0.1, 2)});

		cube_entity->set_local_rotation(util::rand_vec3(-360, 360));
	}
}

void mesh_loading_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	auto *player_ent = nexus->get_entity("player");

	player_ent->set_local_position(glm::vec3{3.296, 1.75, 3.571});

	auto *camera = player_ent->get_component<forge::CameraComponent>();

	camera->pitch = -175;
	camera->yaw = 25;
	player_ent->set_local_rotation(glm::vec3{-175, 25, 175});

	forge::load_meshes_hierarchy(DEMO_ASSET_DIR"models/room_test.glb");
	// forge::load_meshes_hierarchy(DEMO_ASSET_DIR"models/mutli_material.glb");

	// auto *light = nexus->get_entity("Point");
	//
	// if (light)
	// {
	// 	light->set_local_position({2.99, 4.2, 4.7});
	// 	light->get_component<forge::LightComponent>()->get_light()->max_distance = 50;
	// }
	// auto *rug_ent = nexus->get_entity("rug");
	//
	// rug_ent->rotate(160, 0, 0);
}

// this component will make its entities transform become dirty in order to force it to update
struct TransformDirtierComponent : forge::IComponent
{
	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		m_owner->set_local_position(glm::vec3{});
	}
};

struct CounterComponent : forge::IComponent
{
	REGISTER_UPDATE_FUNC

	forge::DeltaTime counter {};

	void update(forge::DeltaTime delta) override
	{
		counter += delta;
	}
};

struct CounterComponent2 : forge::IComponent
{
	REGISTER_UPDATE_FUNC

	forge::DeltaTime counter {};

	void update(forge::DeltaTime delta) override
	{
		counter += delta;
	}
};

struct CounterComponent3 : forge::IComponent
{
	REGISTER_UPDATE_FUNC

	forge::DeltaTime counter {};

	void update(forge::DeltaTime delta) override
	{
		counter += delta;
	}
};

void ecs_stress_test()
{
	constexpr auto TOP_LEVEL_ENTITIES = 5'000;
	auto dirtier_counter = 0;
	auto dirtier_limit = TOP_LEVEL_ENTITIES * 0.002;

	for (int i = 0; i < TOP_LEVEL_ENTITIES; i++)
	{
		auto *entity = g_engine.nexus->create_entity<CounterComponent, CounterComponent2, CounterComponent3>();

		dirtier_counter++;

		if (dirtier_counter >= dirtier_limit)
		{
			entity->add_component<TransformDirtierComponent>();
			dirtier_counter = 0;
		}

		for (int j = 0; j < 4; j++)
		{
			auto *child = entity->emplace_child<CounterComponent2>();

			for (int k = 0; k < 2; k++)
			{
				child->emplace_child<CounterComponent3>();
			}
		}
	}
}

int main(int argc, const char **argv)
{
	auto result = g_engine.init(std::span{argv, (size_t)argc},
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

	forge::register_engine_components();

	register_demo_components();

	auto *editor = g_engine.get_subsystem<forge::EditorSubsystem>();

	editor->demos.emplace("export test", export_test_demo);
	editor->demos.emplace("disappearing cube", diseappring_cube_demo);
	editor->demos.emplace("bobbing cluster", bobbing_cluster);
	editor->demos.emplace("crates", crates_demo);
	editor->demos.emplace("mesh loading", mesh_loading_demo);
	editor->demos.emplace("ecs stress test", ecs_stress_test);

	// editor->load_demo("crates");
	editor->load_demo("mesh loading");
	// editor->load_demo("disappearing cube");

	g_engine.run();

	g_engine.shutdown();
}
