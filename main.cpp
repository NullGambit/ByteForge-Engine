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
	}

	EXPORT_FIELDS(max_health, m_health, m_damage_reduction)

	float max_health = 100;
private:
	float m_health = max_health;
	float m_damage_reduction = 0.5;
};

void print_field(const ecs::ComponentField& field)
{
	fmt::print("{} = ", field.name);

	std::string value;

	switch (field.var.index())
	{
		case 0:
		{
			value = std::to_string(*std::get<0>(field.var));
			break;
		}
	}

	fmt::println("{}", value);
}

int main()
{
	HealthComponent health;

	for (auto &field : health.export_fields())
	{
		print_field(field);
	}
	// ecs::Nexus nexus;
	//
	// auto start = std::chrono::high_resolution_clock::now();
	//
	// for (int i = 0; i < 1'000'000; i++)
	// {
	// 	auto *entity = nexus.create_entity();
	//
	// 	entity->add_component<HealthComponent>(entity);
	//
	// 	nexus.remove_component<HealthComponent>(entity);
	// 	nexus.delete_entity(entity);
	// }
	//
	// auto end = std::chrono::high_resolution_clock::now();
	//
	// std::cout << "Create time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';
	//
	// start = std::chrono::high_resolution_clock::now();
	//
	// nexus.update();
	//
	// end = std::chrono::high_resolution_clock::now();
	//
	// std::cout << "Update time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << '\n';
}
