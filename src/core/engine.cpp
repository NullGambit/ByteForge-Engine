#pragma once

#include "engine.hpp"

#include <ranges>
#include <thread>

#include "fmt/fmt.hpp"
#include "system/window_sub_system.hpp"
#include "system/window.hpp"
#include "graphics/ogl_renderer/ogl_render_sub_system.hpp"
#include "util/types.hpp"

void forge::Engine::quit()
{
	window.set_should_close(true);
}

// TODO: currently if a user subsystem is added it will be added before the builtin subsystems which might not be desired behavior
// TODO: find some way to defer initialization to a later point
bool forge::Engine::init(const EngineInitOptions &options)
{
	m_init_options = options;

	add_subsystem<WindowSubSystem>();

	renderer = add_subsystem<OglRenderSubSystem>();
	nexus = add_subsystem<Nexus>();
	fs_monitor = add_subsystem<FsMonitor>();

	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem == nullptr)
		{
			fmt::println("invalid subsystem found");
			return false;
		}

		auto result = subsystem->init();

		if (!result.empty())
		{
			auto name = util::type_name(typeid(*subsystem));

			fmt::println("Failed to initialize subsystem ({}): {}", name, result);

			if (subsystem->is_critical())
			{
				return false;
			}
		}
	}

	return true;
}

void forge::Engine::run()
{
	start_threaded_subsystems();

	while (window.should_stay_open())
	{
		for (const auto &subsystem : m_subsystems)
		{
			if (subsystem->should_update() && subsystem->get_thread_mode() == SubSystemThreadMode::MainThread)
			{
				subsystem->update();
			}
		}

		window.swap_buffers();
	}
}

void forge::Engine::shutdown()
{
	stop_threaded_subsystems();

	// shutdown and destructs subsystems in reverse order in which they were initialized to ensure correct cleanup
	for (auto &subsystem : std::ranges::reverse_view(m_subsystems))
	{
		subsystem->shutdown();
		subsystem.reset();
	}
}

void forge::Engine::start_threaded_subsystems()
{
	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem->get_thread_mode() == SubSystemThreadMode::SeparateThread)
		{
			m_update_threads.emplace_back(&ISubSystem::threaded_update, subsystem.get());
		}
	}
}

void forge::Engine::stop_threaded_subsystems()
{
	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem->get_thread_mode() == SubSystemThreadMode::SeparateThread)
		{
			subsystem->stop();
		}
	}

	for (auto &thread : m_update_threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}
