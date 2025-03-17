#pragma once

#include <functional>

#include "signal.hpp"
#include "forge/container/map.hpp"
#include "forge/memory/mem_pool.hpp"

namespace forge
{
	using TimerClock = std::chrono::high_resolution_clock;
	using TimerTimePoint = std::chrono::time_point<TimerClock>;
	using TimerDuration = std::chrono::duration<float>;

	struct TimerOptions
	{
		TimerDuration time_sec;
		TimerDuration remaining;
		Delegate<void()> on_timeout {};
		bool one_shot = false;

		inline void invalidate()
		{
			remaining = std::numeric_limits<TimerDuration>::max();
		}

		inline bool is_valid() const
		{
			return remaining != std::numeric_limits<TimerDuration>::max();
		}
	};

	struct TimerHandle
	{

	};

	using TimerID = u32;

	class Timer
	{
	public:
		static constexpr size_t DEFAULT_MAX = 1024;

		Timer(size_t max_timers = DEFAULT_MAX);

		TimerID add(TimerOptions &&options);

		void stop(TimerID id);

		void process();
	private:
		MemPool m_timers;

		TimerTimePoint m_previous_time {};
	};
}
