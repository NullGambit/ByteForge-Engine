#include <iostream>
#include <typeindex>

#include "core/engine.hpp"
#include "src/ecs/ecs.hpp"
#include "src/events/signal.hpp"
#include "src/fmt/fmt.hpp"
#include "src/memory/mem_pool.hpp"
#include "src/util/types.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "src/system/window.hpp"
#include "src/system/window_sub_system.hpp"

int main()
{
	auto &engine = forge::Engine::get_instance();

	auto ok = engine.window.open("ByteForge Engine", 720, 480);

	if (!ok)
	{
		fmt::fatal("could not open window");
	}

	engine.run();

	engine.shutdown();
}
