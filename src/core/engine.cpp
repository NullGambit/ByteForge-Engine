#include "engine.hpp"

#include <condition_variable>
#include <ranges>
#include <set>
#include <thread>

#include "logging.hpp"
#include "concurrency/channel.hpp"
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
	// when constructing dependencies sometimes the unique pointer will be moved mid-dependency construction
	// and it will result in dereferencing a null pointer
	// reserving a large enough amount of memory is a temporary hack until i change how subsystems are stored
	// to avoid smart pointers all together
	m_subsystems.reserve(64);

	add_subsystem<WindowSubSystem>();
	add_subsystem<OglRenderer>();
	add_subsystem<Nexus>();
	add_subsystem<ImGuiSubsystem>();
	add_subsystem<EditorSubsystem>();
}

forge::Engine::~Engine()
{
	for (auto &thread : m_update_threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
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

		if (!initialized_subsystems.contains(type_index) && !m_subsystem_table.contains(type_index))
		{
			auto &ptr = m_subsystems.emplace_back(dependency.construct());

			initialize_subsystem(initialized_subsystems, ptr);

			m_subsystem_table.emplace(type_index, ptr.get());
		}
	}

	if (initialized_subsystems.contains(subsystem_type))
	{
		return EngineInitResult::Ok;
	}

	std::string result;

	if (subsystem->get_thread_mode() == SubSystemThreadMode::OffloadThread)
	{
		auto done = false;

		subsystem->post_event([&subsystem, &result, &done]
		{
			result = subsystem->init();
			done = true;
		});

		while (!done);
	}
	else
	{
		result = subsystem->init();
	}

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

	m_sync_data.stage = SubSystemStage::Init;

	start_subsystems();

	m_sync_data.counter = m_offload_systems;

	ArgParser default_parser;
	ArgParser *parser = options.arg_parser;

	if (!parser)
	{
		parser = &default_parser;
	}

	std::atomic_uint32_t counter = 0;

	for (auto &subsystem : m_subsystems)
	{
		if (subsystem->get_thread_mode() == SubSystemThreadMode::OffloadThread)
		{
			counter++;

			subsystem->post_event([&subsystem, &parser, &counter]
			{
				subsystem->receive_cmd_args(*parser);
				counter--;
			});
		}
		else
		{
			subsystem->receive_cmd_args(*parser);
		}
	}

	while (counter > 0);

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
	// dont fall for clang-tidy's lies future me!!!
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

void forge::Engine::update_subsystems(SubSystemUpdateType type)
{
	std::unique_lock lock { m_sync_data.mutex };

	m_sync_data.update_type = type;

	start_offload_threads();

	for (const auto &subsystem : m_main_thread_subsystems)
	{
		switch (type)
		{
			case SubSystemUpdateType::PreUpdate:
				subsystem->pre_update();
				break;
			case SubSystemUpdateType::Update:
				subsystem->update();
				break;
			case SubSystemUpdateType::PostUpdate:
				subsystem->post_update();
				break;
		}
	}

	m_sync_data.cv_done.wait(lock, [&offload_counter = m_sync_data.counter]{ return offload_counter <= 0; });
}

void forge::Engine::run()
{
	m_sync_data.stage = SubSystemStage::Update;

	while (window.should_stay_open())
	{
		auto current_time = get_engine_runtime();

		m_delta_time = current_time - m_previous_time;
		m_delta_time *= time_scale;
		m_previous_time = current_time;
		m_fps = 1 / m_delta_time;

		update_subsystems(SubSystemUpdateType::PreUpdate);
		update_subsystems(SubSystemUpdateType::Update);

		window.reset_input();

		update_subsystems(SubSystemUpdateType::PostUpdate);

		// TODO: this should swap all window buffers
		window.swap_buffers();
	}

	// start one last time so the offload threads won't get stuck waiting
	start_offload_threads();

}

void forge::Engine::shutdown()
{
	m_sync_data.counter = m_offload_systems;

	stop_threaded_subsystems();

	m_sync_data.stage = SubSystemStage::Shutdown;

	// shutdown and destructs subsystems in reverse order in which they were initialized to ensure correct cleanup
	for (auto &subsystem : std::ranges::reverse_view(m_subsystems))
	{
		if (subsystem->get_thread_mode() == SubSystemThreadMode::OffloadThread)
		{
			subsystem->post_event([&subsystem]
			{
				subsystem->shutdown();
				subsystem.reset();
			});
		}
		else
		{
			subsystem->shutdown();
			subsystem.reset();
		}
	}

	m_sync_data.stage = SubSystemStage::Done;

	std::unique_lock lock { m_sync_data.mutex };

	m_sync_data.cv_done.wait(lock, [&offload_counter = m_sync_data.counter]{ return offload_counter <= 0; });

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
		subsystem->set_sync_data(&m_sync_data);

		if (subsystem->get_thread_mode() == SubSystemThreadMode::SeparateThread)
		{
			m_update_threads.emplace_back(&ISubSystem::threaded_update, subsystem.get());
		}
		else if (subsystem->get_thread_mode() == SubSystemThreadMode::OffloadThread)
		{
			m_update_threads.emplace_back(&ISubSystem::offload_update, subsystem.get());

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
	m_sync_data.counter = m_offload_systems;
	m_sync_data.should_start = true;
	m_sync_data.cv_start.notify_all();
}
