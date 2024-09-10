#pragma once

#include "window_sub_system.hpp"

#include "GLFW/glfw3.h"

void forge::WindowSubSystem::init()
{
	glfwInit();
}

void forge::WindowSubSystem::shutdown()
{
	glfwTerminate();
}

void forge::WindowSubSystem::update()
{
	glfwPollEvents();
}
