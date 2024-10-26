#include "arena_allocator.hpp"

#include "mem_utils.hpp"
#include "core/logging.hpp"
#include "system/virtual_memory.hpp"

forge::ArenaAllocator::ArenaAllocator(size_t max_size)
{
	m_memory = virtual_alloc(max_size);
}

forge::ArenaAllocator::~ArenaAllocator()
{
	virtual_free(m_memory);
}

u8* forge::ArenaAllocator::allocate(size_t size)
{
	size = align_to(size, sizeof(size_t));

	if (auto offset = find_free_header(size); offset)
	{
		return m_memory + offset.value();
	}

	auto *pointer = m_memory + m_offset;

	auto *header = reinterpret_cast<Header*>(pointer);

	header->size = size;
	header->offset = m_offset + sizeof(Header);

	m_offset += size + sizeof(Header);

	return pointer + sizeof(Header);
}

void forge::ArenaAllocator::deallocate(u8* pointer)
{
	auto *header = reinterpret_cast<Header*>(pointer - sizeof(Header));

	m_free_list.emplace(header->size, header->offset);
}

void forge::ArenaAllocator::reset()
{
	m_offset = 0;
	m_free_list.clear();
}

std::optional<u32> forge::ArenaAllocator::find_free_header(u32 size)
{
	auto iter = m_free_list.lower_bound(size);

	if (iter == m_free_list.end())
	{
		return std::nullopt;
	}

	auto offset = iter->second;

	m_free_list.erase(iter);

	return offset;
}
