#include "ecs.hpp"

#include <sys/mman.h>

void ecs::Nexus::ComponentType::update(DeltaTime delta) const
{
	auto *memory = mem_pool.memory();

	for (int i = 0; i < mem_pool.length(); i++)
	{
		auto *component = (BaseComponent*)memory;

		if (component->m_is_active)
		{
			component->update(delta);
		}

		memory += mem_pool.element_size();
	}
}
