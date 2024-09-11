#pragma once

#include "engine.hpp"

#include "fmt/fmt.hpp"
#include "system/window_sub_system.hpp"
#include "system/window.hpp"
#include "graphics/ogl_renderer/ogl_render_sub_system.hpp"

void forge::Engine::quit()
{
	window.set_should_close(true);
}

void forge::Engine::init(const EngineInitOptions &options)
{
	m_init_options = options;

	add_subsystem<WindowSubSystem>();

	renderer = add_subsystem<OglRenderSubSystem>();

	for (const auto &subsystem : m_subsystems)
	{
		subsystem->init();

		if (!subsystem->is_ok())
		{
			fmt::println("Failed to initialize subsystem: {}", subsystem->error_message());
		}
	}
}

void forge::Engine::run()
{
	while (window.should_stay_open())
	{
		for (const auto &subsystem : m_subsystems)
		{
			subsystem->update();
		}

		window.swap_buffers();
	}
}

void forge::Engine::shutdown()
{
	for (const auto &subsystem : m_subsystems)
	{
		subsystem->shutdown();
	}
}
