#pragma once

#include "mem_pool.hpp"

#include <sys/mman.h>

#include "../fmt/fmt.hpp"

bool forge::MemPool::init(size_t element_size, size_t map_size)
{
	m_map_size = map_size;

	m_memory = (uint8_t*)mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (m_memory == MAP_FAILED)
	{
		return false;
	}

	m_element_size = element_size;
	m_offset = 0;
	m_length = 0;

	return true;
}

forge::MemPool::~MemPool()
{
	destroy();
}

void forge::MemPool::destroy()
{
	if (m_memory)
	{
		munmap(m_memory, m_map_size);
	}
}

forge::MemPoolObject forge::MemPool::allocate()
{
	if (!m_free_list.empty())
	{
		auto free_offset = m_free_list.back();
		m_free_list.pop_back();
		return {m_memory + free_offset, free_offset};
	}

	auto *out_mem = m_memory + m_offset;
	auto mem_offset = m_offset;

	++m_length;
	m_offset += m_element_size;

	return {out_mem, mem_offset};
}

void forge::MemPool::free(size_t offset_to_free)
{
	m_free_list.push_back(offset_to_free);
}

// TODO: add an option to call destructors
void forge::MemPool::reset()
{
	m_offset = 0;
	m_length = 0;
	m_free_list.clear();
}
