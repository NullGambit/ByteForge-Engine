#include "engine.hpp"

#include <condition_variable>
#include <ranges>
#include <set>
#include <thread>

#include "logging.hpp"
#include "forge/editor/editor_subsystem.hpp"
#include "forge/system/window_sub_system.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"
#include "GLFW/glfw3.h"
#include "forge/gui/imgui_subsystem.hpp"
#include "forge/system/fs_monitor.hpp"
#include "forge/util/types.hpp"
#include "engine_init_options.hpp"

forge::Engine::Engine()
{
	// when constructing dependencies sometimes the unique pointer will be moved mid-dependency construction
	// and it will result in dereferencing a null pointer
	// reserving a large enough amount of memory is a temporary hack until i change how subsystems are stored
	// to avoid smart pointers all together
	m_subsystems.reserve(64);

	add_subsystem<OglRenderer>();
	add_subsystem<WindowSubSystem>();
	add_subsystem<Nexus>();
	add_subsystem<EditorSubsystem>();
}

forge::Engine::~Engine()
{

}

void forge::Engine::quit()
{
	m_should_run = false;
}

forge::EngineInitResult forge::Engine::initialize_subsystem(std::set<std::type_index> &initialized_subsystems,
	const std::unique_ptr<ISubSystem> &subsystem, const EngineInitOptions &options)
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
			auto &ptr = m_subsystems.emplace_back(dependency.construct());

			initialize_subsystem(initialized_subsystems, ptr, options);

			m_subsystem_table.emplace(type_index, ptr.get());
		}
	}

	if (initialized_subsystems.contains(subsystem_type))
	{
		return EngineInitResult::Ok;
	}

	auto result = subsystem->init(options);

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
	init_logger(options);

	ArgParser default_parser;
	ArgParser *parser = options.arg_parser;

	if (!parser)
	{
		parser = &default_parser;
	}

	for (auto &subsystem : m_subsystems)
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
	// dont fall for clang-tidy's lies future me!!!
	for (auto i = 0; i < m_subsystems.size(); i++)
	{
		auto result = initialize_subsystem(initialized_subsystems, m_subsystems[i], options);

		if (result != EngineInitResult::Ok)
		{
			return result;
		}
	}

	std::stable_sort(m_subsystems.begin(), m_subsystems.end(),
		[](const auto &l, const auto &r)
		{
			return l->get_update_order() < r->get_update_order();
		});

	return EngineInitResult::Ok;
}

void forge::Engine::run()
{
	while (m_should_run)
	{
		auto current_time = get_engine_runtime();

		m_delta_time = current_time - m_previous_time;
		m_delta_time *= time_scale;
		m_previous_time = current_time;
		m_fps = 1 / m_delta_time;

		for (auto &subsystem : m_subsystems)
		{
			if (subsystem->should_update()) [[likely]]
			{
				subsystem->pre_update();
			}
		}

		for (auto &subsystem : m_subsystems)
		{
			if (subsystem->should_update()) [[likely]]
			{
				subsystem->update();
			}
		}

		for (auto &subsystem : m_subsystems)
		{
			if (subsystem->should_update()) [[likely]]
			{
				subsystem->post_update();
			}
		}
	}
}

void forge::Engine::shutdown()
{
	// shutdown and destructs subsystems in reverse order in which they were initialized to ensure correct cleanup
	for (auto &subsystem : std::ranges::reverse_view(m_subsystems))
	{
		subsystem->shutdown();
		subsystem.reset();
	}
}

float forge::Engine::get_engine_runtime()
{
	return get_subsystem<WindowSubSystem>()->get_runtime();
}

void forge::Engine::init_logger(const EngineInitOptions &options)
{
	if (!options.log_file.empty())
	{
		log::set_outfile(options.log_file);
	}
	if (!options.log_time_fmt.empty())
	{
		log::set_time_fmt(options.log_time_fmt);
	}

	log::set_flags(options.log_flags);
}
