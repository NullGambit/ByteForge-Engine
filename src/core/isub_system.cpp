#include "isub_system.hpp"
#include "concurrency/channel.hpp"

void forge::ISubSystem::threaded_update()
{
	while (m_threaded_should_run)
	{
		if (should_update() && m_threaded_should_run)
		{
			update();
		}
	}
}

void forge::ISubSystem::offload_update()
{
	while (m_sync_data->stage == SubSystemStage::Init)
	{
		execute_events();
	}

	while (m_threaded_should_run && m_sync_data->stage == SubSystemStage::Update)
	{
		std::unique_lock lock { m_sync_data->mutex };

		m_sync_data->cv_start.wait(lock, [&should_start = m_sync_data->should_start] { return should_start.load(); });

		if (should_update() && m_threaded_should_run)
		{
			switch (m_sync_data->update_type)
			{
				case SubSystemUpdateType::PreUpdate:
					pre_update();
					break;
				case SubSystemUpdateType::Update:
					update();
					break;
				case SubSystemUpdateType::PostUpdate:
					post_update();
					break;
			}
		}

		--m_sync_data->counter;

		if (m_sync_data->counter <= 0)
		{
			m_sync_data->cv_done.notify_one();
			m_sync_data->should_start = false;
		}
	}

	while (m_sync_data->stage == SubSystemStage::Shutdown)
	{
		execute_events();
	}
}

void forge::ISubSystem::execute_events()
{
	m_events.execute_all();

	if (--m_sync_data->counter <= 0)
	{
		m_sync_data->cv_done.notify_one();
		m_sync_data->should_start = false;
	}
}
