
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>
#include <forge/framework/components/mesh_primitive_component.hpp>

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

	g_engine.run();

	g_engine.shutdown();
}
