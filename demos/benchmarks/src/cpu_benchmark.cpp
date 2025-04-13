#include "cpu_benchmark.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

void Benchmarker::run_cases(size_t amount)
{
	using namespace std::chrono;

	for (auto &test_case : cases)
	{
		Duration total {};

		for (size_t i = 0; i < amount; i++)
		{
			auto start = high_resolution_clock::now();

			test_case.on_run();

			auto end = high_resolution_clock::now();

			total += end-start;
		}

		test_case.result = total / amount;
	}

}

void Benchmarker::display_results()
{
	using namespace std::chrono;

	for (const auto &bench_case : cases)
	{
		std::cout << bench_case.label << ": ";
		auto count = bench_case.result.count();

		if (count >= 1000000000)
		{
			std::cout << count / 1000000000 << "s";
		}
		else if (count >= 1000000)
		{
			std::cout << count / 1000000 << "ms";
		}
		else if (count >= 1000)
		{
			std::cout << count / 1000 << "us";
		}
		else
		{
			std::cout << count << "ns";
		}

		std::cout << '\n';
	}
}
