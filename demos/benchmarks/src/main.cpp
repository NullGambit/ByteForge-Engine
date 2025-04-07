
#include <thread>
#include <forge/core/engine.hpp>
#include <forge/core/logging.hpp>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "cpu_benchmark.hpp"
#include "forge/editor/editor_subsystem.hpp"
#include "forge/events/timer.hpp"
#include "forge/framework/components/camera_component.hpp"
#include "forge/framework/components/register_engine_components.hpp"
#include "forge/util/random.hpp"

#define ROCKET_COUNT 200'000
#define TEST_COUNT 5000

struct RocketComponent : forge::IComponent
{
	glm::vec3 position {};
	glm::vec3 velocity {};

	void update(forge::DeltaTime delta) override
	{
		position += glm::vec3{1.0f};
		velocity.y += 1;
	}
};

struct RocketManagerComponent : forge::IComponent
{
	std::vector<glm::vec3> positions {};
	std::vector<glm::vec3> velocities {};

	RocketManagerComponent()
	{
		positions.resize(ROCKET_COUNT);
		velocities.resize(ROCKET_COUNT);
	}

	void update(forge::DeltaTime delta) override
	{
		for (auto i = 0; i < ROCKET_COUNT; i++)
		{
			positions[i] += glm::vec3{1.0f};
			velocities[i].y += 1;
		}
	}
};

static forge::Nexus g_nexus;

void update_nexus()
{
	g_nexus.update();
}

void clear_nexus()
{
	g_nexus.update();
}

int main(int argc, const char **argv)
{
	Benchmarker bench;

	g_nexus.init({});

	bench.cases.emplace_back("component based", []
	{
		for (auto i = 0; i < ROCKET_COUNT; i++)
		{
			g_nexus.create_entity<RocketComponent>();
		}

	}, update_nexus, clear_nexus);

	bench.cases.emplace_back("manager based based", []
	{
		g_nexus.create_entity<RocketManagerComponent>();

	}, update_nexus, clear_nexus);

	bench.run_cases(TEST_COUNT);

	bench.display_results();
	// auto result = g_engine.init(std::span{argv, (size_t)argc},
	// {
	// 	.window_title = "ByteForge Benchmarks",
	// 	.window_width = 1920,
	// 	.window_height = 1080,
	// 	.log_flags = log::LogTime
	// });
	//
	// if (result == forge::EngineInitResult::HaltWithNoError)
	// {
	// 	return 0;
	// }
	// if (result != forge::EngineInitResult::Ok)
	// {
	// 	return -1;
	// }
	//
	// forge::register_engine_components();
	//
	//
	// g_engine.run();
	//
	// g_engine.shutdown();
}
