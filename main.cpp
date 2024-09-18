#include <condition_variable>
#include <iostream>
#include <thread>
#include <typeindex>

#include "core/engine.hpp"
#include "src/util/types.hpp"

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

	engine.run();

	engine.shutdown();
}
