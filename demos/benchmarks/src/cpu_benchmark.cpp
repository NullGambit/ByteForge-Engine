#include "cpu_benchmark.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

void Benchmarker::run_cases(size_t amount)
{
	using namespace std::chrono;

	std::atomic_int32_t remaining {};

	for (auto &test_case : cases)
	{
		std::thread t {[&test_case, amount, &remaining]()
		{
			remaining++;

			Duration total {};

			test_case.on_start();

			for (size_t i = 0; i < amount; i++)
			{
				auto start = high_resolution_clock::now();

				test_case.on_run();

				auto end = high_resolution_clock::now();

				total += end-start;
			}

			test_case.result = total / amount;

			test_case.on_finish();

			remaining--;

		}};

		t.detach();
	}

	while (remaining > 0);
}

void Benchmarker::display_results()
{
	for (const auto &test_case : cases)
	{
		std::cout << test_case.label << ": " << test_case.result << '\n';
	}
}
