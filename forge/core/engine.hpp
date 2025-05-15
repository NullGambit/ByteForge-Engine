#pragma once

#include <memory>
#include <set>
#include <vector>
#include <span>

#include "time.hpp"
#include "forge/ecs/defs.hpp"
#include "forge/core/isub_system.hpp"
#include "forge/system/window.hpp"
#include "forge/container/map.hpp"

namespace forge
{
	class OglRenderer;
	class Window;
	class Nexus;

	enum class EngineInitResult
	{
		Ok,
		// means a controlled shutdown happened and should not be considered an error
		HaltWithNoError,
		ArgParserError,
		SubsystemInitError,
	};

	struct SubsystemTimings
	{
		Duration pre_update {};
		Duration update {};
		Duration post_update {};
	};

	class Engine
	{
	public:

		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

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

		DeltaTime time_scale = 1.0;

		OglRenderer *renderer = nullptr;
		Window *window = nullptr;
		Nexus *nexus = nullptr;

	#ifdef FORGE_RECORD_SUBSYSTEM_TIMINGS
			HashMap<std::type_index, SubsystemTimings> subsystem_timings;
	#endif

	private:
		DeltaTime m_delta_time;
		DeltaTime m_previous_time;
		DeltaTime m_fps;
		bool m_should_run = true;

		// subsystem storage for fast iteration
		std::vector<std::unique_ptr<ISubSystem>> m_subsystems;
		// subsystem storage for lookups. allows users to find subsystems by registered type.
		HashMap<std::type_index, ISubSystem*> m_subsystem_table;

		void init_logger(const EngineInitOptions &options);

		EngineInitResult initialize_subsystem(std::set<std::type_index> &initialized_subsystems,
			const std::unique_ptr<ISubSystem> &subsystem, const EngineInitOptions &options);

		// TODO: make it so that a subsystem can override another one
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
	};
}

inline forge::Engine g_engine;