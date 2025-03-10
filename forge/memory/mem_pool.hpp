#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "forge/core/logging.hpp"
#include "forge/util/types.hpp"

namespace forge
{
	namespace MemPoolFlags
	{
		enum : u8
		{
			Invalid = 1 << 0,
			Valid = 1 << 1
		};
	}

	struct MemPoolObject
	{
		u8 *pointer;
		size_t byte_offset;
	};

	template<class T>
	class MemPoolIterator
	{
	public:
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::forward_iterator_tag;

		MemPoolIterator(u8 *m_offset, const size_t m_element_size) :
			m_offset(m_offset),
			m_element_size(m_element_size)
		{}

		reference operator*() const
		{
			return *(pointer)m_offset;
		}

		pointer operator->() const
		{
			return (pointer)m_offset;
		}

		MemPoolIterator& operator++()
		{
			m_offset += m_element_size;
			return *this;
		}

		MemPoolIterator operator++(int)
		{
			auto temp = *this;
			m_offset += m_element_size;
			return temp;
		}

		bool operator==(const MemPoolIterator &other) const
		{
			return m_offset == other.m_offset;
		}

		bool operator!=(const MemPoolIterator &other) const
		{
			return m_offset != other.m_offset;
		}

	private:
		u8* m_offset;
		const size_t m_element_size;
	};

	class MemPool;

	// a proxy object for the begin and end functions to be defined in
	// this is required because these functions cant be implemented within mempool while still retaining a type
	// mostly just for ranged based for loops
	template<class T>
	class MemPoolTypedIterator
	{
	public:

		MemPoolTypedIterator(const MemPool *mempool) :
			m_mempool(mempool)
		{}

		MemPoolTypedIterator(const MemPool &mempool) :
			m_mempool(&mempool)
		{}

		MemPoolIterator<T> begin();

		MemPoolIterator<T> end();

	private:
		const MemPool *m_mempool;
	};

	class MemPool
	{
		typedef void(*DestroyFunc)(u8*);
		typedef void(*ConstructFunc)(u8*);
		typedef void(*CreateIteratorFunc)(u8*);

	public:
		MemPool() = default;
		~MemPool();

		bool init(size_t element_size, size_t map_size);

		template<class T>
		bool init(size_t map_size)
		{
			set_destructor<T>();
			set_constructor<T>();
			return init(sizeof(T), map_size);
		}

		void destroy();

		MemPoolObject allocate(bool construct = false);

		template<class T>
		void set_destructor()
		{
			m_destroy_func = [](u8 *ptr)
			{
				auto *data = (T*)ptr;
				data->~T();
			};
		}

		template<class T>
		void set_constructor()
		{
			m_construct_func = [](u8 *mem)
			{
				new (mem) T();
			};
		}

		template<class T, class... Args>
		std::pair<T*, size_t> emplace(Args ...args)
		{
			auto [mem, offset] = allocate(false);

			return {new (mem) T(std::forward<Args>(args)...), offset};
		}

		void free(size_t offset_to_free, bool destroy = true);

		void free_at(size_t index, bool destroy = true);

		void reset(bool destroy = true, DestroyFunc on_destroy = nullptr);

		inline u8* get(size_t offset)
		{
			return m_memory + offset;
		}

		template<class T>
		inline T* get(size_t offset)
		{
			return (T*)get(offset);
		}

		inline u8* get_from_index(size_t index)
		{
			return m_memory + m_element_size * index;
		}

		template<class T>
		inline T* get_from_index(size_t index)
		{
			return (T*)get_from_index(index);
		}

		inline size_t get_length() const
		{
			return m_length;
		}

		inline size_t get_offset() const
		{
			return m_offset;
		}

		inline size_t get_element_size() const
		{
			return m_element_size;
		}

		inline u8* get_memory() const
		{
			return m_memory;
		}

		template<class T>
		MemPoolTypedIterator<T> get_iterator() const
		{
			return this;
		}

	private:
		u8 *m_memory;
		size_t m_offset;
		size_t m_length;
		size_t m_element_size;
		size_t m_map_size;
		std::vector<size_t> m_free_list;
		DestroyFunc m_destroy_func = nullptr;
		ConstructFunc m_construct_func = nullptr;
	};

	template<class T>
	MemPoolIterator<T> MemPoolTypedIterator<T>::begin()
	{
		return {m_mempool->get_memory(), m_mempool->get_element_size()};
	}

	template<class T>
	MemPoolIterator<T> MemPoolTypedIterator<T>::end()
	{
		auto end_offset = m_mempool->get_length() * m_mempool->get_element_size();
		return {m_mempool->get_memory() + end_offset, m_mempool->get_element_size()};
	}
}