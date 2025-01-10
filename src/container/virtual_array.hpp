#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>

#include "core/logging.hpp"
#include "memory/defs.hpp"
#include "system/virtual_memory.hpp"
#include "util/types.hpp"

namespace forge
{
	namespace VirtualArrayFlags
	{
		enum : u8
		{
			Free = 1 << 0,
			InUse = 1 << 1
		};
	}

	struct VirtualArrayHeader
	{
		u8 flags;
		u32 next_free;
		u32 offset;
	};

	template<class T>
	class VirtualArrayIterator
	{
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::forward_iterator_tag;

		VirtualArrayIterator(T *memory, const size_t m_element_size) :
			m_memory(memory),
			m_element_size(m_element_size)
		{}

		reference operator*()
		{
			auto *header = (VirtualArrayHeader*)m_memory;

			// if the thing being dereferenced is actually free which would always happen if the first element is free
			// then skip past this and any other free memory
			if (header->flags & VirtualArrayFlags::Free)
			{
				this->operator++();
			}

			return *(pointer)(m_memory + sizeof(VirtualArrayHeader));
		}

		VirtualArrayIterator& operator++()
		{
			m_memory += m_element_size + sizeof(VirtualArrayHeader);

			auto *header = (VirtualArrayHeader*)m_memory;

			bool is_free = header->flags & VirtualArrayFlags::Free;

			while (is_free)
			{
				auto *header = (VirtualArrayHeader*)m_memory;

				is_free = header->flags & VirtualArrayFlags::Free;

				if (is_free)
				{
					m_memory += m_element_size + sizeof(VirtualArrayHeader);
				}
			}

			return *this;
		}

		bool operator==(const VirtualArrayIterator &other) const
		{
			return m_memory == other.m_memory;
		}

		bool operator!=(const VirtualArrayIterator &other) const
		{
			return m_memory != other.m_memory;
		}

	private:
		T* m_memory;
		const size_t m_element_size;
	};

	template<class T>
	class VirtualArray
	{
	public:
		explicit VirtualArray(size_t map_size = KB(16))
		{
			auto adjusted_size = map_size / sizeof(T);

			adjusted_size *= sizeof(VirtualArrayHeader) + sizeof(T);

			m_memory = (T*)virtual_alloc(adjusted_size);

			m_offset = 0;
			m_length = 0;

			m_map_size = adjusted_size;
		}

		~VirtualArray()
		{
			m_offset = 0;
			m_length = 0;
			m_map_size = 0;

			if (m_memory)
			{
				virtual_free((u8*)m_memory);
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

		T& add(T &&data)
		{
			auto [mem, offset] = allocate();

			auto *ptr = new (mem) T(std::forward<T>(data));

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
			auto offset = index * (sizeof(VirtualArrayHeader) + sizeof(T));

			auto *ptr = (T*)m_memory + offset;

			auto *header = (VirtualArrayHeader*)(ptr - sizeof(VirtualArrayHeader));

			if (header->flags & VirtualArrayFlags::Free)
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

		VirtualArrayIterator<T> begin()
		{
			return {m_memory, sizeof(T)};
		}

		VirtualArrayIterator<T> end()
		{
			return {m_memory + m_offset, sizeof(T)};
		}

	private:
		T *m_memory;
		u32 m_offset;
		u32 m_length;
		u32 m_map_size;

		// the start of the first free VirtualArray header
		VirtualArrayHeader *m_free_head = nullptr;
		VirtualArrayHeader *m_free_tail = nullptr;

		void free(u8 *memory)
		{
			auto *header = (VirtualArrayHeader*)(memory - sizeof(VirtualArrayHeader));

			if (header->flags & VirtualArrayFlags::Free)
			{
				return;
			}

			header->flags = (header->flags | VirtualArrayFlags::Free) & ~VirtualArrayFlags::InUse;

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
			constexpr auto header_size = sizeof(VirtualArrayHeader);

			T *memory = nullptr;

			if (m_free_head)
			{
				memory = m_memory + (m_free_head->offset + header_size);

				m_free_head->flags = (m_free_head->flags | VirtualArrayFlags::InUse) & ~VirtualArrayFlags::Free;

				if (m_free_head->next_free != UINT32_MAX)
				{
					auto *next_header = (VirtualArrayHeader*)(m_memory + m_free_head->next_free);
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

				VirtualArrayHeader header
				{
					.flags = VirtualArrayFlags::InUse,
					.next_free = UINT32_MAX,
					.offset = m_offset,
				};

				memcpy(m_memory + m_offset, &header, header_size);

				m_offset += header_size;

				memory = m_memory + m_offset;

				m_offset += sizeof(T);
			}

			m_length++;

			return memory;
		}
	};
}
