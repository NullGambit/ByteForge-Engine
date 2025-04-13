#pragma once
#include <chrono>
#include <functional>
#include <string_view>

struct Benchmarker
{
	using Duration = std::chrono::duration<long, std::ratio<1, 1000000000>>;

	struct Case
	{
		std::string_view label;
		std::function<void()> on_run;
		Duration result {};
	};

	void run_cases(size_t amount);

	void display_results();

	std::vector<Case> cases;
};
