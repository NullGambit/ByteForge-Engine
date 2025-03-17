
#include <thread>
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "components/bob_component.hpp"
#include "components/export_test_component.hpp"
#include "components/player_controller.hpp"
#include "forge/editor/editor_subsystem.hpp"
#include "forge/events/timer.hpp"
#include "forge/framework/components/camera_component.hpp"
#include "forge/framework/components/register_engine_components.hpp"
#include "forge/util/random.hpp"

void register_demo_components()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->register_component<PlayerController>();
	nexus->register_component<BobComponent>();
	nexus->register_component<ExportFieldTestComponent>();
}

void export_test_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->create_entity<ExportFieldTestComponent>();
}

void make_player()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->create_entity<forge::CameraComponent, PlayerController>("player");
}

void cube_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	auto &cube_entity = nexus->create_entity("cube");

	auto *cube = cube_entity.add_component<MeshPrimitiveComponent>();

	cube->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);

	auto cube_position = cube_entity.get_local_position();

	cube_position.x = 3;
	cube_position.y = -1;
	cube_position.z = -2;

	cube_entity.set_local_position(cube_position);
}

void bobbing_cluster()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	make_player();

	constexpr auto COUNT = 100;
	constexpr auto DISTANCE = 25;

	for (auto i = 0; i < COUNT; i++)
	{
		auto &cube_entity = nexus->create_entity(fmt::format("cube_{}", i));

		auto *cube = cube_entity.add_component<MeshPrimitiveComponent>();
		auto *bob = cube_entity.add_component<BobComponent>();

		bob->amplitude = util::rand_float(0.1, 2);
		bob->amplitude = util::rand_float(0.1, 2);

		cube->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);

		cube_entity.set_local_position(util::rand_vec3(-DISTANCE, DISTANCE));

		cube_entity.set_local_scale(glm::vec3{util::rand_float(0.1, 2)});

		cube_entity.set_local_rotation(util::rand_vec3(-360, 360));
	}
}

int main(int argc, const char **argv)
{
	forge::Timer timer;

	forge::Delegate<void(int, float)> test =
		[](int a, float b)
		{
			log::info("{} {}", a, b);
		};

	test(10, 5.5);

	timer.add(forge::TimerOptions
	{
		.time_sec = std::chrono::milliseconds(500),
		.on_timeout = []
		{
			log::info("hello");
		},
		.one_shot = true
	});

	for (auto i = 0; i < 50; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{100});
		timer.process();
	}

	// auto result = g_engine.init(std::span{argv, (size_t)argc},
	// {
	// 	.window_title = "ByteForge Engine",
	// 	.window_width = 1920,
	// 	.window_height = 1080,
	// 	.log_flags = log::LogTime
	// });
	//
	// if (result == forge::EngineInitResult::HaltWithNoError)
	// {
	// 	return 0;
	// }
	// if (result != forge::EngineInitResult::Ok)
	// {
	// 	return -1;
	// }
	//
	// forge::register_engine_components();
	//
	// register_demo_components();
	//
	// auto *editor = g_engine.get_subsystem<forge::EditorSubsystem>();
	//
	// editor->demos.emplace_back("export test", export_test_demo);
	// editor->demos.emplace_back("cube", cube_demo);
	// editor->demos.emplace_back("bobbing cluster", bobbing_cluster);
	//
	// g_engine.run();
	//
	// g_engine.shutdown();
}
