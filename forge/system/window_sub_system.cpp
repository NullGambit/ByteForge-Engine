#include "window_sub_system.hpp"
#include "forge/core/engine.hpp"
#include "GLFW/glfw3.h"

std::string forge::WindowSubSystem::init(const EngineInitOptions &options)
{
	auto ok = glfwInit();

	if (!ok)
	{
		return "could not init glfw";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	g_main_window = &m_windows.emplace();

	ok = g_main_window->open(options.window_title, options.window_width, options.window_height);

	if (!ok)
	{
		return "could not open main window";
	}

	return {};
}

void forge::WindowSubSystem::shutdown()
{
	for (auto &window : m_windows)
	{
		window.close();
	}
	glfwTerminate();
}

void forge::WindowSubSystem::update()
{
}

void forge::WindowSubSystem::pre_update()
{
	glfwPollEvents();
}

void forge::WindowSubSystem::post_update()
{
	for (auto &window : m_windows)
	{
		window.swap_buffers();
		window.reset_input();
	}
}

forge::Window * forge::WindowSubSystem::create_window()
{
	return &m_windows.emplace();
}

void forge::WindowSubSystem::destroy_window(Window *window)
{
	assert(window != g_main_window);

	m_windows.free(window);
}

forge::Window * forge::WindowSubSystem::get_main_window()
{
	return m_windows.get(0);
}

float forge::WindowSubSystem::get_runtime() const
{
	return glfwGetTime();
}
