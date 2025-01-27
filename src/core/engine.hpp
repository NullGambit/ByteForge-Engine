#pragma once

#include <memory>
#include <set>
#include <vector>
#include <thread>

#include "isub_system.hpp"
#include "config/arg_parser.hpp"
#include "ecs/ecs.hpp"
#include "system/window.hpp"

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
		ArgParser *arg_parser = nullptr;
	};

	enum class EngineInitResult
	{
		Ok,
		HaltWithNoError,
		ArgParserError,
		SubsystemInitError,
	};

	class Engine
	{
	public:

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static inline Engine& get_instance()
		{
			static Engine instance;
			return instance;
		}

		void quit();

		EngineInitResult init(std::span<const char*> sys_args, const EngineInitOptions &options);


		void run();

		void shutdown();

		[[nodiscard]]
		float get_engine_runtime();

		[[nodiscard]]
		inline float get_fps() const
		{
			return m_fps;
		}

		[[nodiscard]]
		inline float get_delta() const
		{
			return m_delta_time;
		}

		[[nodiscard]]
		const EngineInitOptions& get_init_options() const
		{
			return m_init_options;
		}

		template<class T, class ...Args>
		inline T* add_subsystem(Args &&...args) requires std::derived_from<T, ISubSystem>
		{
			return add_subsystem_implementation<T, T>(std::make_unique<T>(std::forward<Args>(args)...));
		}

		template<class T, class I, class ...Args>
		inline T* add_subsystem_as(Args &&...args) requires std::derived_from<T, ISubSystem>
		{
			return add_subsystem_implementation<T, I>(std::make_unique<T>(std::forward<Args>(args)...));
		}

		template<class T>
		inline T* get_subsystem()
		{
			auto iter = m_subsystem_table.find(typeid(T));

			if (iter == m_subsystem_table.end())
			{
				return nullptr;
			}

			return (T*)iter->second;
		}

		template<class T>
		inline bool is_subsystem_initialized() const
		{
			return m_subsystem_table.contains(typeid(T));
		}

		// TODO: create some sort of window factory class that will create windows and will swap their buffers in the main loop
		Window window;
		DeltaTime time_scale = 1.0;

	private:
		// subsystem storage for fast iteration
		std::vector<std::unique_ptr<ISubSystem>> m_subsystems;
		// subsystem storage for lookups. allows users to find subsystems by registered type.
		HashMap<std::type_index, ISubSystem*> m_subsystem_table;
		EngineInitOptions m_init_options;
		// threads for subsystems with a SeparateThread mode
		std::vector<std::thread> m_update_threads;
		std::vector<ISubSystem*> m_main_thread_subsystems;

		SubSystemSyncData m_sync_data;
		u32 m_offload_systems;

		float m_delta_time;
		float m_previous_time;
		float m_fps;

		Engine();
		~Engine();

		void init_logger();

		void start_subsystems();
		void stop_threaded_subsystems();
		void start_offload_threads();

		EngineInitResult initialize_subsystem(std::set<std::type_index> &initialized_subsystems,
			const std::unique_ptr<ISubSystem> &subsystem);

		template<class T, class I>
		inline T* add_subsystem_implementation(std::unique_ptr<ISubSystem> &&subsystem) requires std::derived_from<T, ISubSystem>
		{
			if (m_subsystem_table.contains(typeid(I)))
			{
				return nullptr;
			}

			auto *ptr = (T*)m_subsystems.emplace_back(std::forward<std::unique_ptr<ISubSystem>>(subsystem)).get();

			assert(ptr != nullptr);

			m_subsystem_table.emplace(typeid(I), ptr);

			return ptr;
		}

		void update_subsystems(SubSystemUpdateType type);
	};
}
