#include "time.hpp"

forge::String forge::to_string(Duration duration)
{
	const auto count = duration.count();

	if (count >= 1000000000.0f)
	{
		return std::to_string(count / 1000000000) + "s";
	}
	if (count >= 1000000.0f)
	{
		return std::to_string(count / 1000000) + "ms";
	}
	if (count >= 1000.0f)
	{
		return std::to_string(count / 1000) + "us";
	}

	return std::to_string(count) + "ns";
}
