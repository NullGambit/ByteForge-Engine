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
		// will be updated on a different thread but will wait until all other main and offload threads are done updating to start the next frame
		OffloadThread,
		// will run on its own thread and won't wait for other subsystems to finish updating
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
		virtual SubSystemThreadMode get_thread_mode() { return SubSystemThreadMode::MainThread; }

		// if true the system init call must succeed or else the engine will fail to init
		virtual bool is_critical() { return false; }

		// will be called if the thread mode is set to SeparateThread
		void threaded_update()
		{
			while (m_threaded_should_run)
			{
				if (should_update())
				{
					update();
				}
			}
		}

		// will be called if the thread mode is set to OffloadThread
		void offload_update(std::atomic_bool &should_start, std::atomic_int &counter,
			std::condition_variable &cv_start, std::condition_variable &cv_done, std::mutex &mutex)
		{
			while (m_threaded_should_run)
			{
				std::unique_lock lock { mutex };

				cv_start.wait(lock, [&should_start] { return should_start.load(); });

				if (should_update())
				{
					update();
				}

				if (--counter <= 0)
				{
					cv_done.notify_one();
					should_start = false;
				}
			}
		}

	private:
		std::atomic_bool m_threaded_should_run = true;
	};
}
