#include <iostream>
#include <typeindex>

#include "src/ecs/ecs.hpp"
#include "src/events/signal.hpp"
#include "src/fmt/fmt.hpp"
#include "src/memory/mem_pool.hpp"
#include "src/util/types.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "src/system/window.hpp"

int main()
{
	forge::Window window;

	auto ok = window.open("ByteForge Engine", 720, 480);

	if (!ok)
	{
		fmt::fatal("could not open window");
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fmt::fatal("could init glad");
	}

	glClearColor(255, 0, 0, 255);

	while (window.should_stay_open())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		window.handle_events();
		window.swap_buffers();
	}
}
