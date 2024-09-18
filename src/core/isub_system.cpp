#include "isub_system.hpp"


void forge::ISubSystem::threaded_update()
{
	while (m_threaded_should_run)
	{
		if (should_update())
		{
			update();
		}
	}
}

void forge::ISubSystem::offload_update(std::atomic_bool& should_start, std::atomic_int& counter,
	std::condition_variable& cv_start, std::condition_variable& cv_done, std::mutex& mutex)
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
