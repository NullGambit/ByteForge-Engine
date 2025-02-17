#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

#include "forge/core/logging.hpp"
#include "forge/memory/defs.hpp"
#include "forge/system/virtual_memory.hpp"
#include "forge/util/types.hpp"

#define DEFAULT_VIRTUAL_ARRAY_SIZE KB(4)

namespace forge
{
	// an array backed by virtual memory that does not grow or shrink
	template<class T>
	class VirtualArray
	{
		class Iterator;

		enum : u8
		{
			FREE = 1 << 0,
			IN_USE = 1 << 1
		};

		struct Header
		{
			u8 flags;
			u32 next_free;
			u32 offset;
		};

	public:
		explicit VirtualArray(i32 max_elements = -1)
		{
			if (max_elements == -1)
			{
				auto base = DEFAULT_VIRTUAL_ARRAY_SIZE / sizeof(T);
				m_map_size = base * (sizeof(Header) + sizeof(T));
			}
			else
			{
				m_map_size = max_elements * (sizeof(Header) + sizeof(T));
			}

			m_memory = virtual_alloc(m_map_size);

			m_offset = 0;
			m_length = 0;
		}

		~VirtualArray()
		{
			m_offset = 0;
			m_length = 0;
			m_map_size = 0;

			if (m_memory)
			{
				virtual_free(m_memory);
				m_memory = nullptr;
			}
		}

		template<class ...Args>
		T& emplace(Args &&...args)
		{
			auto *mem = allocate();

			auto *ptr = new (mem) T(std::forward<Args>(args)...);

			return *ptr;
		}

		void free(T *ptr, bool destroy = true)
		{
			free((u8*)ptr);

			if (destroy)
			{
				ptr->~T();
			}
		}

		void clear(bool destroy = true)
		{
			if (destroy)
			{
				for (auto &element : this)
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
		// very unreliable dont use it please.
		inline T* get(size_t index)
		{
			auto offset = index * (sizeof(Header) + sizeof(T));

			auto *ptr = (T*)m_memory + offset;

			auto *header = (Header*)(ptr - sizeof(Header));

			if (header->flags & FREE)
			{
				return nullptr;
			}

			return ptr;
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

		Iterator begin()
		{
			return {m_memory, sizeof(T)};
		}

		Iterator end()
		{
			return {m_memory + m_offset, sizeof(T)};
		}

	private:
		u8 *m_memory;
		u32 m_offset;
		u32 m_length;
		u32 m_map_size;

		// the start of the first free VirtualArray header
		Header *m_free_head = nullptr;
		Header *m_free_tail = nullptr;

		void free(u8 *memory)
		{
			auto *header = (Header*)(memory - sizeof(Header));

			if (header->flags & FREE)
			{
				return;
			}

			header->flags = (header->flags | FREE) & ~IN_USE;

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

		T* allocate()
		{
			constexpr auto header_size = sizeof(Header);

			u8 *memory = nullptr;

			if (m_free_head)
			{
				memory = m_memory + (m_free_head->offset + header_size);

				m_free_head->flags = (m_free_head->flags | IN_USE) & ~FREE;

				if (m_free_head->next_free != UINT32_MAX)
				{
					auto *next_header = (Header*)(m_memory + m_free_head->next_free);
					m_free_head = next_header;
				}
				else
				{
					m_free_head = nullptr;
				}
			}
			else
			{
				assert(m_offset + header_size + sizeof(T) <= m_map_size && "VirtualArray size exceeded limit");

				Header header
				{
					.flags = IN_USE,
					.next_free = UINT32_MAX,
					.offset = m_offset,
				};

				memcpy(m_memory + m_offset, &header, header_size);

				m_offset += header_size;

				memory = m_memory + m_offset;

				m_offset += sizeof(T);
			}

			m_length++;

			return (T*)memory;
		}
	};

	template<class T>
	class VirtualArray<T>::Iterator
	{
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::forward_iterator_tag;

		Iterator(u8 *memory, const size_t m_element_size) :
			m_memory(memory),
			m_element_size(m_element_size)
		{}

		reference operator*()
		{
			auto *header = (Header*)m_memory;

			// if the thing being dereferenced is actually free which would always happen if the first element is free
			// then skip past this and any other free memory
			if (header->flags & FREE)
			{
				this->operator++();
			}

			return *(pointer)(m_memory + sizeof(Header));
		}

		Iterator& operator++()
		{
			m_memory += m_element_size + sizeof(Header);

			auto *header = (Header*)m_memory;

			bool is_free = header->flags & FREE;

			while (is_free)
			{
				auto *header = (Header*)m_memory;

				is_free = header->flags & FREE;

				if (is_free)
				{
					m_memory += m_element_size + sizeof(Header);
				}
			}

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
		u8* m_memory;
		const size_t m_element_size;
	};
}
