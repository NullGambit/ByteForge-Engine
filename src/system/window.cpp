#include "window.hpp"

#include <GLFW/glfw3.h>

#include "fmt/fmt.hpp"

void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	windowPtr->on_resize(width, height);
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

	set_context();

	glfwSetWindowUserPointer(m_handle, this);
	glfwSetFramebufferSizeCallback(m_handle, framebuffer_resize_callback);

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

void forge::Window::set_should_close(bool value)
{
	glfwSetWindowShouldClose(m_handle, value);
}

void forge::Window::swap_buffers() const
{
	glfwSwapBuffers(m_handle);
}

void forge::Window::set_context()
{
	glfwMakeContextCurrent(m_handle);
}

GLFWwindow* forge::Window::get_handle() const
{
	return m_handle;
}
