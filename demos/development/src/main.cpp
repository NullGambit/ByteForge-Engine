
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "components/bob_component.hpp"
#include "components/export_test_component.hpp"
#include "components/player_controller.hpp"
#include "forge/editor/editor_subsystem.hpp"
#include "forge/framework/components/register_engine_components.hpp"

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

void cube_demo()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	auto &cube_entity = nexus->create_entity("cube");

	auto *cube = cube_entity.add_component<MeshPrimitiveComponent>();

	cube->set_texture(FORGE_ENGINE_ASSET_DIR"textures/container2.png", forge::TextureType::Diffuse);

	auto cube_position = cube_entity.get_local_position();

	cube_position.x = 3;

	cube_entity.set_local_position(cube_position);
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

	editor->demos.emplace_back("export test", export_test_demo);
	editor->demos.emplace_back("cube", cube_demo);

	g_engine.run();

	g_engine.shutdown();
}
