
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "components/bob_component.hpp"
#include "components/export_test_component.hpp"
#include "components/player_controller.hpp"
#include "forge/framework/components/register_engine_components.hpp"

void register_demo_components()
{
	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->register_component<PlayerController>();
	nexus->register_component<BobComponent>();
	nexus->register_component<ExportFieldTestComponent>();
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

	auto *nexus = g_engine.get_subsystem<forge::Nexus>();

	nexus->create_entity<ExportFieldTestComponent>();

	g_engine.run();

	g_engine.shutdown();
}
