#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

#include "forge/core/logging.hpp"
#include "forge/memory/defs.hpp"
#include "forge/memory/virtual_allocator.hpp"
#include "forge/system/virtual_memory.hpp"
#include "forge/util/types.hpp"

#define DEFAULT_VIRTUAL_ARRAY_SIZE KB(4)

namespace forge
{
	// an array backed by virtual memory that does not grow or shrink
	template<class T, class Allocator = VirtualAllocator<T>>
	class VirtualArray
	{
		struct Header
		{
			alignas(T)
			u8 memory[sizeof(T)];
			bool is_free = true;
			u32 next_free;
			u32 offset;
		};

	public:
		class Iterator;

		struct AllocateResult
		{
			T *ptr = nullptr;
			bool reused = false;
		};

		bool is_initialized() const
		{
			return m_memory != nullptr;
		}

		void init(i32 max_elements = -1)
		{
			if (max_elements == -1)
			{
				m_map_size = DEFAULT_VIRTUAL_ARRAY_SIZE;
			}
			else
			{
				m_map_size = max_elements * sizeof(Header);
			}

			m_memory = m_allocator.allocate_bytes(m_map_size);

			m_offset = 0;
			m_length = 0;
		}

		void destroy()
		{
			m_offset = 0;
			m_length = 0;
			m_map_size = 0;

			if (m_memory)
			{
				m_allocator.deallocate_bytes(m_memory, m_map_size);
				m_memory = nullptr;
			}
		}

		~VirtualArray()
		{
			destroy();
		}

		Iterator begin() const
		{
			return m_memory;
		}

		Iterator end() const
		{
			return m_memory + m_offset;
		}

		template<class ...Args>
		T& emplace(Args &&...args)
		{
			auto [mem, _] = allocate();

			auto *ptr = new (mem) T(std::forward<Args>(args)...);

			return *ptr;
		}

		AllocateResult allocate()
		{
			AllocateResult result;

			if (m_memory == nullptr)
			{
				init(DEFAULT_VIRTUAL_ARRAY_SIZE);
			}

			constexpr auto header_size = sizeof(Header);

			Header *header;

			if (m_free_head)
			{
				header = (Header*)(m_memory + m_free_head->offset);

				m_free_head->is_free = false;

				if (m_free_head->next_free != UINT32_MAX)
				{
					auto *next_header = (Header*)(m_memory + m_free_head->next_free);
					m_free_head = next_header;
				}
				else
				{
					m_free_head = nullptr;
				}

				result.reused = true;
			}
			else
			{
				assert(m_offset + header_size <= m_map_size && "VirtualArray size exceeded limit");

				header = (Header*)(m_memory + m_offset);

				*header =
				{
					.is_free = false,
					.next_free = UINT32_MAX,
					.offset = m_offset,
				};

				m_offset += header_size;
			}

			m_length++;

			result.ptr = (T*)header->memory;

			return result;
		}

		void free(T *memory, bool destroy = true)
		{
			if (destroy)
			{
				memory->~T();
			}

			auto *header = get_header(memory);

			if (header->is_free)
			{
				return;
			}

			header->is_free = true;

			if (m_free_head == nullptr)
			{
				m_free_head = header;
				m_free_tail = header;
			}
			else
			{
				m_free_tail->next_free = header->offset;
				m_free_tail = header;
			}

			m_length--;
		}

		void clear(bool destroy = true)
		{
			if (destroy)
			{
				for (auto &element : *this)
				{
					element.~T();
				}
			}

			m_offset = 0;
			m_length = 0;

			m_free_head = nullptr;
			m_free_tail = nullptr;
		}

		// will retrieve an element by index. if index has already been freed will return nullptr
		// element could be freed already. use with care.
		inline T* get(size_t index)
		{
			auto offset = index * sizeof(Header);

			return get_from_offset(offset);
		}

		inline const T* get(size_t index) const
		{
			auto offset = index * sizeof(Header);

			return get_from_offset(offset);
		}

		inline T* get_from_offset(size_t offset)
		{
			auto *header = (Header*)(m_memory + offset);

			if (header->is_free)
			{
				return nullptr;
			}

			return (T*)header->memory;
		}

		inline bool is_free(T *mem) const
		{
			return get_header(mem)->is_free;
		}

		inline u32 get_offset_of_mem(T *mem) const
		{
			return get_header(mem)->offset;
		}

		inline u32 get_index_of_mem(T *mem) const
		{
			return get_offset_of_mem(mem) / sizeof(Header);
		}

		inline size_t get_length() const
		{
			return m_length;
		}

		inline size_t get_offset() const
		{
			return m_offset;
		}

		inline size_t get_memory_limits() const
		{
			return m_map_size;
		}

		inline u8* get_memory() const
		{
			return m_memory;
		}

	private:
		u8 *m_memory = nullptr;
		u32 m_offset = 0;
		u32 m_length = 0;
		u32 m_map_size = 0;
		Allocator m_allocator;

		// the start of the first free VirtualArray header
		Header *m_free_head = nullptr;
		Header *m_free_tail = nullptr;

		Header* get_header(T *mem)
		{
			return (Header*)((u8*)mem - offsetof(Header, memory));
		}
	};

	template<class T, class Allocator>
	class VirtualArray<T, Allocator>::Iterator
	{
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::forward_iterator_tag;

		Iterator(u8 *memory) :
			m_memory(memory)
		{}

		reference operator*()
		{
			return *(pointer)((Header*)m_memory)->memory;
		}

		Iterator& operator++()
		{
			Header *header;

			do
			{
				m_memory += sizeof(Header);

				header = (Header*)m_memory;

			} while (header->is_free);

			return *this;
		}

		bool operator==(const Iterator &other) const
		{
			return m_memory == other.m_memory;
		}

		bool operator!=(const Iterator &other) const
		{
			return m_memory != other.m_memory;
		}

	private:
		u8 *m_memory;
	};
}
