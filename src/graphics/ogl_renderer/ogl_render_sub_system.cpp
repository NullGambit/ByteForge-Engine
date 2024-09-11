#pragma once

#include "ogl_render_sub_system.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

void forge::OglRenderSubSystem::init()
{
	m_ok = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!m_ok)
	{
		m_error_message = "could not init glad";
		return;
	}
}

void forge::OglRenderSubSystem::update()
{
}

void forge::OglRenderSubSystem::shutdown()
{
}
