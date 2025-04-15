#pragma once

#include <memory>
#include <typeindex>
#include <vector>

#include "forge/config/arg_parser.hpp"
#include "engine_init_options.hpp"

namespace forge
{
	class ISubSystem;

	class DependencyStorage
	{
	public:
		typedef std::unique_ptr<ISubSystem>(*ConstructTFunc)();

		DependencyStorage(std::type_index index) :
			m_type_index(index)
		{}

		template<class T>
		void init()
		{
			m_construct_func = [] -> std::unique_ptr<ISubSystem>
			{
				return std::make_unique<T>();
			};
		}

		[[nodiscard]]
		std::unique_ptr<ISubSystem> construct() const
		{
			return m_construct_func();
		}

		[[nodiscard]]
		const std::type_index& get_type_index() const
		{
			return m_type_index;
		}

	private:
		ConstructTFunc m_construct_func;
		std::type_index m_type_index;
	};

	template<class T>
	DependencyStorage make_dependency()
	{
		DependencyStorage storage { typeid(T) };

		storage.init<T>();

		return storage;
	}

	class ISubSystem
	{
	public:
		virtual ~ISubSystem() = default;

		// if the returning string is not empty that means an error occurred
		virtual std::string init(const EngineInitOptions &options) = 0;
		virtual void shutdown() = 0;
		virtual void update() = 0;

		// the order in which this subsystem will get updated when compared to other subsystems
		// applies to all update events.
		virtual u32 get_update_order() { return 0; }

		// allows for subsystems to add their own flags to the command line parser for per subsystem configuration
		virtual void receive_cmd_args(ArgParser &parser) {}

		// dependencies must be initialized otherwise this subsystem won't get initialized
		virtual std::vector<DependencyStorage> get_dependencies() { return {}; }

		// will be called at the start of the update loop
		virtual void pre_update() {}
		// will be called at the end of the update loop
		virtual void post_update() {}

		// called right before the engine loop.
		virtual void on_run() {}

		virtual bool should_update() { return true; }

		// if true the system init call must succeed or else the engine will fail to init
		[[nodiscard]]
		virtual bool is_critical() { return false; }
	};
}
