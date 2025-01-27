#pragma once
#include <atomic>
#include <condition_variable>
#include <string_view>
#include <typeindex>
#include <vector>

#include "concurrency/command_buffer.hpp"
#include "config/arg_parser.hpp"

namespace forge
{
	struct MultiChannel;

	enum class SubSystemThreadMode
	{
		// will run on the main thread
		MainThread,
		// will be updated on a different thread but will wait until the main and all other offload threads are done updating to start the next frame
		OffloadThread,
		// will run on its own thread as often as it can
		SeparateThread,
	};

	enum class SubSystemUpdateType
	{
		PreUpdate,
		Update,
		PostUpdate,
	};

	// represents the 3 stages a subsystem will be in and which events can potentially be run
	enum class SubSystemStage
	{
		Init,
		Update,
		Shutdown,
		Done,
	};

	struct SubSystemSyncData
	{
		SubSystemStage stage;
		SubSystemUpdateType update_type;
		std::atomic_bool should_start;
		std::atomic_int counter;
		MultiChannel *channel;
		std::condition_variable cv_start;
		std::condition_variable cv_done;
		std::mutex mutex;
	};

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
	inline DependencyStorage make_dependency()
	{
		DependencyStorage storage { typeid(T) };

		storage.init<T>();

		return storage;
	}

	class ISubSystem
	{
	public:
		virtual ~ISubSystem() = default;

		void set_sync_data(SubSystemSyncData *sync_data)
		{
			m_sync_data = sync_data;
		}

		void post_event(CommandBuffer<>::Callback &&event)
		{
			m_events.emplace(std::forward<CommandBuffer<>::Callback>(event));
		}

		// if the returning string is not empty that means an error occurred
		virtual std::string init() = 0;
		virtual void shutdown() = 0;
		virtual void update() = 0;

		// allows for subsystems to add their own flags to the command line parser for per subsystem configuration
		virtual void receive_cmd_args(ArgParser &parser) {}

		// dependencies must be initialized otherwise this subsystem won't get initialized
		virtual std::vector<DependencyStorage> get_dependencies() { return {}; }

		// will be called at the start of the update loop
		virtual void pre_update() {}
		// will be called at the end of the update loop
		virtual void post_update() {}

		virtual bool should_update() { return true; }

		inline void stop()
		{
			m_threaded_should_run = false;
		}

		// will set the thread mode at the beginning of the run loop
		[[nodiscard]]
		virtual inline SubSystemThreadMode get_thread_mode() { return SubSystemThreadMode::MainThread; }

		// if true the system init call must succeed or else the engine will fail to init
		[[nodiscard]]
		virtual bool is_critical() { return false; }

		// will be called if the thread mode is set to SeparateThread
		void threaded_update();

		// will be called if the thread mode is set to OffloadThread
		void offload_update();

	private:
		CommandBuffer<> m_events;
		std::atomic_bool m_threaded_should_run = true;
		SubSystemSyncData *m_sync_data;

		void execute_events();
	};
}
