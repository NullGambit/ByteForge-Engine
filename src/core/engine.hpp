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
	class OglRenderer;
}

namespace forge
{
	struct EngineInitOptions
	{
		std::string_view window_title;
		int window_width;
		int window_height;
		uint8_t log_flags;
		std::string_view log_file;
		std::string_view log_time_fmt;
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

		OglRenderer *renderer;
		Nexus *nexus;
		FsMonitor *fs_monitor;
		// TODO: create some sort of window factory class that will create windows and will swap their buffers in the main loop
		Window window;
	private:
		std::vector<std::unique_ptr<ISubSystem>> m_subsystems;
		EngineInitOptions m_init_options;
		// threads for subsystems with a SeparateThread mode
		std::vector<std::thread> m_update_threads;
		std::vector<ISubSystem*> m_main_thread_subsystems;

		// ======== member variables for offload subsystems ========

		std::mutex m_offload_mutex;
		std::condition_variable m_cv_start;
		std::condition_variable m_cv_done;
		int m_offload_systems = 0;
		std::atomic_int m_offload_counter = 0;
		std::atomic_bool m_should_start = false;

		// ==============================================

		void start_subsystems();
		void stop_threaded_subsystems();

		void start_offload_threads();
	};
}
