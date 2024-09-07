#pragma once

#include "ecs.hpp"

#include <sys/mman.h>

ecs::EcsResult ecs::Nexus::ComponentType::init(size_t size)
{
	memory = (uint8_t*)mmap(nullptr, ECS_MAX_MAPPED_MEMORY, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (memory == MAP_FAILED)
	{
		return EcsResult::CouldNotAllocateComponentMemory;
	}

	component_size = size;
	offset = 0;
	length = 0;

	return EcsResult::Ok;
}

void ecs::Nexus::ComponentType::destroy()
{
	munmap(memory, ECS_MAX_MAPPED_MEMORY);
}

std::pair<uint8_t*, size_t> ecs::Nexus::ComponentType::allocate()
{
	if (!free_list.empty())
	{
		auto free_offset = free_list.back();
		free_list.pop_back();
		return {memory + free_offset, free_offset};
	}

	auto *out_mem = memory + offset;
	auto mem_offset = offset;

	++length;
	offset += component_size;

	return {out_mem, mem_offset};
}

void ecs::Nexus::ComponentType::free(size_t offset_to_free)
{
	free_list.push_back(offset_to_free);
}

void ecs::Nexus::ComponentType::update(DeltaTime delta) const
{
	for (size_t i = 0; i < offset; i++)
	{
		auto *component = (BaseComponent*)memory + i;

		if (component->m_is_active)
		{
			component->update(delta);
		}

		i += component_size;
	}
}
