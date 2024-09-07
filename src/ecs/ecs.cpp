#include "ecs.hpp"

#include <sys/mman.h>

void ecs::Nexus::ComponentType::update(DeltaTime delta) const
{
	for (size_t i = 0; i < mem_pool.offset(); i++)
	{
		auto *component = (BaseComponent*)mem_pool.memory() + i;

		if (component->m_is_active)
		{
			component->update(delta);
		}

		i += mem_pool.element_size();
	}
}
