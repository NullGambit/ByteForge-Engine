#pragma once

#include <functional>

#include "signal.hpp"
#include "forge/container/map.hpp"
#include "forge/core/time.hpp"
#include "forge/memory/mem_pool.hpp"

namespace forge
{
	struct TimerOptions
	{
		Duration duration;
		Duration remaining;
		Delegate<void()> on_timeout {};
		bool one_shot = false;

		inline void invalidate()
		{
			remaining = std::numeric_limits<Duration>::max();
		}

		inline bool is_valid() const
		{
			return remaining != std::numeric_limits<Duration>::max();
		}
	};

    // TODO: implement a scoped timer handle
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

		TimePoint m_previous_time {};
	};
}
