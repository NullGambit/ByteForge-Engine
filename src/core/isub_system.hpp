#pragma once
#include <atomic>
#include <condition_variable>
#include <string_view>

#include "fmt/fmt.hpp"

namespace forge
{
	enum class SubSystemThreadMode
	{
		// will run on the main thread
		MainThread,
		// will be updated on a different thread but will wait until the main and all other offload threads are done updating to start the next frame
		OffloadThread,
		// will run on its own thread as often as it can
		SeparateThread,
	};

	class ISubSystem
	{
	public:
		virtual ~ISubSystem() = default;

		// if the returning string is not empty that means an error occurred
		virtual std::string init() = 0;
		virtual void shutdown() = 0;
		virtual void update() = 0;

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
		void offload_update(std::atomic_bool &should_start, std::atomic_int &counter,
			std::condition_variable &cv_start, std::condition_variable &cv_done, std::mutex &mutex);

	private:
		std::atomic_bool m_threaded_should_run = true;
	};
}
