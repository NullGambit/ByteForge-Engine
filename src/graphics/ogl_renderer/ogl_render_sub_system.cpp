#pragma once

#include "ogl_render_sub_system.hpp"

#include "core/engine.hpp"
#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

void forge::OglRenderSubSystem::init()
{
	m_ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!m_ok)
	{
		m_error_message = "could not init glad";
		return;
	}

	glClearColor(255, 0, 0, 1);

	auto &engine = Engine::get_instance();

	// TODO: remove connection
	engine.window.on_resize.connect(this, &OglRenderSubSystem::handle_framebuffer_resize);
}

void forge::OglRenderSubSystem::update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void forge::OglRenderSubSystem::shutdown()
{
}

void forge::OglRenderSubSystem::handle_framebuffer_resize(int width, int height)
{
	glViewport(0, 0, width, height);
}
