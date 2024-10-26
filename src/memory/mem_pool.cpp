#include "mem_pool.hpp"

#include <cassert>
#include <unistd.h>
#include <sys/mman.h>
#include <rpmalloc/rpmalloc.h>

#include "mem_utils.hpp"
#include "../fmt/fmt.hpp"
#include "system/virtual_memory.hpp"
#include "system/system_info.hpp"

bool forge::MemPool::init(size_t element_size, size_t map_size)
{
	m_map_size = align_to(map_size, get_page_size());

	m_memory = virtual_alloc(map_size);

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
		m_memory = nullptr;
	}
}

forge::MemPoolObject forge::MemPool::allocate(bool construct)
{
	auto offset = m_offset;

	if (!m_free_list.empty())
	{
		offset = m_free_list.back();
		m_free_list.pop_back();
	}

	auto *out_mem = m_memory + offset;
	auto mem_offset = offset;

	++m_length;
	m_offset += m_element_size;

	if (construct && m_construct_func)
	{
		m_construct_func(out_mem);
	}

	return {out_mem, mem_offset};
}

void forge::MemPool::free(size_t offset_to_free, bool destroy)
{
	if (destroy && m_destroy_func)
	{
		m_destroy_func(m_memory + offset_to_free);
	}

	m_free_list.push_back(offset_to_free);
}

void forge::MemPool::reset(bool destroy)
{
	if (destroy && m_destroy_func)
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
