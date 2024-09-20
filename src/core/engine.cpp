#include "engine.hpp"

#include <condition_variable>
#include <ranges>
#include <thread>

#include "logging.hpp"
#include "fmt/fmt.hpp"
#include "system/window_sub_system.hpp"
#include "system/window.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"
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

	if (!m_init_options.log_file.empty())
	{
		log::set_outfile(m_init_options.log_file);
	}

	log::set_flags(m_init_options.log_flags);

	add_subsystem<WindowSubSystem>();

	fs_monitor = add_subsystem<FsMonitor>();
	renderer = add_subsystem<OglRenderer>();
	nexus = add_subsystem<Nexus>();

	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem == nullptr)
		{
			fmt::println("invalid subsystem found");
			return false;
		}

		auto result = subsystem->init();

		auto name = util::type_name(typeid(*subsystem));

		if (!result.empty())
		{
			log::warn("Failed to initialize subsystem ({}): {}", name, result);

			if (subsystem->is_critical())
			{
				return false;
			}
		}
		else
		{
			log::info("Initialized subsystem {}", name);
		}
	}

	return true;
}

void forge::Engine::run()
{
	start_subsystems();

	while (window.should_stay_open())
	{
		// start offload threads update function
		start_offload_threads();

		// updates main thread subsystems
		for (const auto &subsystem : m_main_thread_subsystems)
		{
			if (subsystem->should_update())
			{
				subsystem->update();
			}
		}

		// TODO: this should swap all window buffers
		window.swap_buffers();

		// waits till all offload threads are done
		std::unique_lock lock { m_offload_mutex };
		m_cv_done.wait(lock, [&offload_counter = m_offload_counter]{ return offload_counter <= 0; });
	}

	// start one last time so the offload threads won't get stuck waiting
	start_offload_threads();
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

void forge::Engine::start_subsystems()
{
	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem->get_thread_mode() == SubSystemThreadMode::SeparateThread)
		{
			m_update_threads.emplace_back(&ISubSystem::threaded_update, subsystem.get());
		}
		else if (subsystem->get_thread_mode() == SubSystemThreadMode::OffloadThread)
		{
			m_update_threads.emplace_back(&ISubSystem::offload_update, subsystem.get(),
				std::ref(m_should_start), std::ref(m_offload_counter),
				std::ref(m_cv_start), std::ref(m_cv_done),
				std::ref(m_offload_mutex));

			++m_offload_systems;
		}
		else
		{
			m_main_thread_subsystems.emplace_back(subsystem.get());
		}
	}
}

void forge::Engine::stop_threaded_subsystems()
{
	for (const auto &subsystem : m_subsystems)
	{
		if (subsystem->get_thread_mode() != SubSystemThreadMode::MainThread)
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

void forge::Engine::start_offload_threads()
{
	std::unique_lock lock { m_offload_mutex };
	m_offload_counter = m_offload_systems;
	m_should_start = true;
	m_cv_start.notify_all();
}
