#pragma once
#include <chrono>

#include "forge/container/string.hpp"

namespace forge
{
	using Nanoseconds = std::chrono::nanoseconds;
	using Microseconds = std::chrono::microseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Seconds = std::chrono::seconds;
	using Minutes = std::chrono::minutes;
	using Hours = std::chrono::hours;
	using Days = std::chrono::days;
	using Weeks = std::chrono::weeks;
	using Months = std::chrono::months;
	using Years = std::chrono::years;

	using Duration = Nanoseconds;
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	String to_string(Duration duration);
}
