#include "engine.hpp"

#include <condition_variable>
#include <ranges>
#include <set>
#include <thread>

#include "logging.hpp"
#include "editor/editor_subsystem.hpp"
#include "system/window_sub_system.hpp"
#include "system/window.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"
#include "GLFW/glfw3.h"
#include "gui/imgui_subsystem.hpp"
#include "system/fs_monitor.hpp"
#include "util/types.hpp"

forge::Engine::Engine()
{
	add_subsystem<WindowSubSystem>();
	add_subsystem<OglRenderer>();
	add_subsystem<Nexus>();
	add_subsystem<ImGuiSubsystem>();
	add_subsystem<EditorSubsystem>();
}

void forge::Engine::quit()
{
	window.set_should_close(true);
}

forge::EngineInitResult forge::Engine::initialize_subsystem(std::set<std::type_index> &initialized_subsystems,
	const std::unique_ptr<ISubSystem> &subsystem)
{
	if (subsystem == nullptr)
	{
		log::warn("invalid subsystem found");
		return EngineInitResult::SubsystemInitError;
	}

	auto &subsystem_type = typeid(*subsystem);

	auto name = util::type_name(subsystem_type);

	for (auto dependency : subsystem->get_dependencies())
	{
		const auto &type_index = dependency.get_type_index();

		if (!initialized_subsystems.contains(type_index))
		{
			if (!m_subsystem_table.contains(type_index))
			{
				auto &ptr = m_subsystems.emplace_back(dependency.construct());

				initialize_subsystem(initialized_subsystems, ptr);

				m_subsystem_table.emplace(type_index, ptr.get());
			}
		}
	}

	if (initialized_subsystems.contains(subsystem_type))
	{
		return EngineInitResult::Ok;
	}

	auto result = subsystem->init();

	if (!result.empty())
	{
		log::warn("Failed to initialize subsystem ({}): {}", name, result);

		if (subsystem->is_critical())
		{
			return EngineInitResult::SubsystemInitError;
		}
	}
	else
	{
		log::info("Initialized subsystem {}", name);
		initialized_subsystems.emplace(subsystem_type);
	}

	return EngineInitResult::Ok;
}

// TODO: currently if a user subsystem is added it will be added before the builtin subsystems which might not be desired behavior
// TODO: find some way to defer initialization to a later point
forge::EngineInitResult forge::Engine::init(std::span<const char*> sys_args, const EngineInitOptions &options)
{
	m_init_options = options;

	init_logger();

	ArgParser default_parser;

	ArgParser *parser = options.arg_parser;

	if (parser == nullptr)
	{
		parser = &default_parser;
	}

	for (const auto &subsystem : m_subsystems)
	{
		subsystem->receive_cmd_args(*parser);
	}

	auto result = parser->parse(sys_args);

	if (result == "h")
	{
		return EngineInitResult::HaltWithNoError;
	}
	if (!result.empty())
	{
		log::fatal(result);
		return EngineInitResult::ArgParserError;
	}

	std::set<std::type_index> initialized_subsystems;

	// range based for loop is not a good idea because a new subsystem might be added to the list
	for (auto i = 0; i < m_subsystems.size(); i++)
	{
		auto result = initialize_subsystem(initialized_subsystems, m_subsystems[i]);

		if (result != EngineInitResult::Ok)
		{
			return result;
		}
	}

	return EngineInitResult::Ok;
}

void forge::Engine::run()
{
	start_subsystems();

	while (window.should_stay_open())
	{
		auto current_time = get_engine_runtime();

		m_delta_time = current_time - m_previous_time;
		m_delta_time *= time_scale;
		m_previous_time = current_time;
		m_fps = 1 / m_delta_time;

		for (const auto &subsystem : m_subsystems)
		{
			subsystem->start_tick();
		}

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

		// waits till all offload threads are done
		std::unique_lock lock { m_offload_mutex };
		m_cv_done.wait(lock, [&offload_counter = m_offload_counter]{ return offload_counter <= 0; });

		window.reset_input();

		for (const auto &subsystem : m_subsystems)
		{
			subsystem->end_tick();
		}

		// TODO: this should swap all window buffers
		window.swap_buffers();
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

	log::info("Engine shutdown successfully");
}

float forge::Engine::get_engine_runtime()
{
	return get_subsystem<WindowSubSystem>()->get_runtime();
}

void forge::Engine::init_logger()
{
	if (!m_init_options.log_file.empty())
	{
		log::set_outfile(m_init_options.log_file);
	}
	if (!m_init_options.log_time_fmt.empty())
	{
		log::set_time_fmt(m_init_options.log_time_fmt);
	}

	log::set_flags(m_init_options.log_flags);
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
