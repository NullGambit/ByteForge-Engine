#include "window_sub_system.hpp"
#include "core/engine.hpp"
#include "GLFW/glfw3.h"

std::string forge::WindowSubSystem::init()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	auto ok = glfwInit();

	if (!ok)
	{
		return "could not init glfw";
	}

	auto &engine = Engine::get_instance();
	const auto &options = engine.get_init_options();

	ok = engine.window.open(options.window_title, options.window_width, options.window_height);

	if (!ok)
	{
		return "could not open main window";
	}

	return {};
}

void forge::WindowSubSystem::shutdown()
{
	glfwTerminate();
}

void forge::WindowSubSystem::update()
{
}

void forge::WindowSubSystem::post_update()
{
	glfwPollEvents();
}

void forge::WindowSubSystem::poll_events()
{

}

float forge::WindowSubSystem::get_runtime() const
{
	return glfwGetTime();
}
