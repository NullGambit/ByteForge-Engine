#pragma once

#include "window.hpp"

#include <GLFW/glfw3.h>

forge::Window::Window()
{


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

forge::Window::~Window()
{
	close();
}

void forge::Window::close()
{
	if (m_handle)
	{
		glfwDestroyWindow(m_handle);
	}
	m_handle = nullptr;
}

bool forge::Window::open(std::string_view name, int width, int height)
{
	m_handle = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);

	if (!m_handle)
	{
		return false;
	}

	glfwMakeContextCurrent(m_handle);

	return true;
}

bool forge::Window::should_close() const
{
	return glfwWindowShouldClose(m_handle);
}

bool forge::Window::should_stay_open() const
{
	return !glfwWindowShouldClose(m_handle);
}

void forge::Window::swap_buffers() const
{
	glfwSwapBuffers(m_handle);
}
