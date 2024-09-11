#pragma once

#include "window_sub_system.hpp"

#include "core/engine.hpp"
#include "GLFW/glfw3.h"

void forge::WindowSubSystem::init()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_ok = glfwInit();

	if (!m_ok)
	{
		m_error = "could not init glfw";
	}

	auto &engine = Engine::get_instance();
	const auto &options = engine.get_init_options();

	m_ok = engine.window.open(options.window_title, options.window_width, options.window_height);

	if (!m_ok)
	{
		m_error = "could not open main window";
	}
}

void forge::WindowSubSystem::shutdown()
{
	glfwTerminate();
}

void forge::WindowSubSystem::update()
{
	glfwPollEvents();
}

bool forge::WindowSubSystem::is_ok() const
{
	return m_ok;
}

std::string_view forge::WindowSubSystem::error_message() const
{
	return m_error;
}
