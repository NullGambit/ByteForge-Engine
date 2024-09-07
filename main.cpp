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
		fmt::println("updated health");
	}

};

int main()
{
	ecs::Nexus nexus;

	for (int i = 0; i < 100'000; i++)
	{
		auto *entity = nexus.create_entity();

		nexus.add_component<MyData>(entity);
		nexus.add_component<HealthComponent>(entity, true);

		auto *data = nexus.get_component<MyData>(entity);

		fmt::print("{} {}\n", data->x, data->s);
	}


	nexus.update();
}
