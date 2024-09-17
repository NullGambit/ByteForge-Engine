#pragma once

#include <memory>
#include <vector>
#include <thread>

#include "isub_system.hpp"
#include "ecs/ecs.hpp"
#include "system/fs_monitor.hpp"
#include "system/window.hpp"

namespace forge
{
	class OglRenderSubSystem;
}

namespace forge
{
	struct EngineInitOptions
	{
		std::string_view window_title;
		int window_width;
		int window_height;
	};

	class Engine
	{
	public:

		Engine() = default;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static inline Engine& get_instance()
		{
			static Engine instance;
			return instance;
		}

		void quit();

		bool init(const EngineInitOptions &options);

		void run();

		void shutdown();

		const EngineInitOptions& get_init_options() const
		{
			return m_init_options;
		}

		template<class T, class ...Args>
		inline T* add_subsystem(Args ...args) requires std::derived_from<T, ISubSystem>
		{
			return (T*)m_subsystems.emplace_back(std::make_unique<T>(args...)).get();
		}

		OglRenderSubSystem *renderer;
		Nexus *nexus;
		FsMonitor *fs_monitor;
		// TODO: create some sort of window factory class that will create windows and will swap their buffers in the main loop
		Window window;
	private:
		std::vector<std::unique_ptr<ISubSystem>> m_subsystems;
		EngineInitOptions m_init_options;
		// threads for subsystems with a SeparateThread mode
		std::vector<std::thread> m_update_threads;

		void start_threaded_subsystems();
		void stop_threaded_subsystems();
	};
}
