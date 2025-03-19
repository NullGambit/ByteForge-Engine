#include "timer.hpp"

#include "forge/core/engine.hpp"

forge::Timer::Timer(size_t max_timers)
{
	m_timers.init<TimerOptions>(max_timers * sizeof(TimerOptions));
}

forge::TimerID forge::Timer::add(TimerOptions &&options)
{
	auto [ptr, id] = m_timers.emplace<TimerOptions>(std::forward<TimerOptions>(options));
	ptr->remaining = ptr->duration;
	return id;
}

void forge::Timer::stop(TimerID id)
{
	auto *ptr = m_timers.get<TimerOptions>(id);

	if (ptr == nullptr || !ptr->is_valid())
	{
		return;
	}

	ptr->invalidate();

	m_timers.free(id);
}

void forge::Timer::process()
{
	auto current_time = TimerClock::now();
	auto delta_time = current_time - m_previous_time;
	m_previous_time = current_time;

	size_t offset {};

	for (auto &timer : m_timers.get_iterator<TimerOptions>())
	{
		if (timer.is_valid())
		{
			timer.remaining -= delta_time;

			if (timer.remaining.count() <= 0)
			{
				timer.on_timeout();

				if (timer.one_shot)
				{
					stop(offset);
				}
				else
				{
					timer.remaining = timer.duration;
				}
			}
		}

		offset += sizeof(TimerOptions);
	}
}
