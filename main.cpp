#include <iostream>
#include <typeindex>

#include "src/ecs/ecs.hpp"
#include "src/fmt/fmt.hpp"
#include "src/memory/mem_pool.hpp"
#include "src/util/types.hpp"

struct MyData
{
	int x = 10;
	std::string s = "Hello World!";
};

class HealthComponent : public ecs::BaseComponent
{
public:
	void update(ecs::DeltaTime delta) override
	{
		m_health += 1;
		// fmt::println("updated health");
	}
	float m_health;
};

int main()
{
	ecs::Nexus nexus;

	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1'000'000; i++)
	{
		auto *entity = nexus.create_entity();

		nexus.add_component<MyData>(entity);
		nexus.add_component<HealthComponent>(entity, true);
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Create time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';

	start = std::chrono::high_resolution_clock::now();

	nexus.update();

	end = std::chrono::high_resolution_clock::now();

	std::cout << "Update time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';
}
