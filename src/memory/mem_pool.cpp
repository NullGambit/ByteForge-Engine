#include "mem_pool.hpp"

#include <sys/mman.h>

#include "../fmt/fmt.hpp"
#include "system/virtual_memory.hpp"

bool forge::MemPool::init(size_t element_size, size_t map_size)
{
	m_map_size = map_size;

	m_memory = virtual_alloc(map_size);

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
		virtual_free(m_memory);
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

void forge::MemPool::reset(bool destroy)
{
	if (destroy)
	{
		for (size_t i = 0; i < m_length; i++)
		{
			auto *ptr = m_memory + i * m_element_size;

			m_destroy_func(ptr);
		}
	}

	m_offset = 0;
	m_length = 0;
	m_free_list.clear();
}
