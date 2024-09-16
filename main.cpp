#include <iostream>
#include <thread>
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
#include "src/events/type_box.hpp"
#include "system/fs_monitor.hpp"
#include "util/macros.hpp"

int main()
{
	// auto &engine = forge::Engine::get_instance();
	//
	// auto ok = engine.init(
	// {
	// 	.window_title = "ByteForge Engine",
	// 	.window_width = 720,
	// 	.window_height = 480,
	// });
	//
	// if (!ok)
	// {
	// 	return -1;
	// }
	//
	// engine.run();
	//
	// engine.shutdown();
	forge::FsMonitor monitor;

	monitor.add_watch("./", forge::FSE_FILE_CREATE, [](auto events, auto path)
	{
		fmt::println("modified path {}", path);
	});

	while (true)
	{
		monitor.poll();
	}
}
