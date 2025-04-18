

#include <iostream>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "cpu_benchmark.hpp"
#include "forge/editor/editor_subsystem.hpp"

#include "forge/util/random.hpp"


class Base
{
public:
	virtual void update() {}
};

class Derived : public Base
{
public:
	int counter = 0;

	void update() override
	{
		counter++;
	}
};

template<class T>
class CrtBase
{
	void update()
	{
		((T*)this)->update();
	}
};

class CrtDerived : public CrtBase<CrtDerived>
{
public:
	// char padding[10];
	int counter = 0;

	void update()
	{
		counter++;
	}
};

int main()
{
	Benchmarker bench;

	constexpr auto COUNT = 10'000'000;

	std::vector<Derived> derived_counters;
	derived_counters.resize(COUNT);

	std::vector<CrtDerived> crt_counters;
	crt_counters.resize(COUNT);

	bench.cases.emplace_back("Derived",
	[&derived_counters]
	{
		for (auto &counter : derived_counters)
		{
			counter.update();
		}
	});

	bench.cases.emplace_back("CRT",
	[&crt_counters]
	{
		for (auto &counter : crt_counters)
		{
			counter.update();
		}
	});

	bench.run_cases(100);
	bench.display_results();
}
