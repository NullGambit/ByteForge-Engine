#pragma once

#include <cstddef>
#include <map>
#include <mutex>
#include <optional>

namespace forge
{
	class ArenaAllocator;

	template<class T>
	struct ScopedMemory
	{
		ArenaAllocator *allocator;
		T *data;

		~ScopedMemory();
	};

	class ArenaAllocator
	{
		struct Header
		{
			u32 size;
			u32 offset;
		};

	public:

		ArenaAllocator(size_t max_size);
		~ArenaAllocator();

		u8* allocate(size_t size);

		template<class T>
		inline T* allocate()
		{
			auto *pointer = allocate(sizeof(T));

			return new (pointer) T;
		}

		void deallocate(u8 *pointer);

		template<class T>
		inline void deallocate(T *pointer)
		{
			pointer->~T();

			deallocate((u8*)pointer);
		}

		void reset();

	private:
		u8     *m_memory = nullptr;
		size_t	m_offset = 0;
		std::multimap<u32, u32> m_free_list;

		std::optional<u32> find_free_header(u32 size);
	};

	template<class T>
	ScopedMemory<T>::~ScopedMemory()
	{
		allocator->deallocate(data);
	}
}
