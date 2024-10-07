#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "core/logging.hpp"

namespace forge
{
	struct MemPoolObject
	{
		u8 *pointer;
		size_t byte_offset;
	};

	class MemPool
	{
		typedef void(*DestroyFunc)(u8*);
		typedef void(*ConstructFunc)(u8*);

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

		MemPool(const MemPool& other) :
			  m_memory{other.m_memory},
			  m_offset{other.m_offset},
			  m_length{other.m_length},
			  m_element_size{other.m_element_size},
			  m_map_size{other.m_map_size},
			  m_free_list{other.m_free_list}
		{}

		MemPool(MemPool&& other) noexcept :
			  m_memory{other.m_memory},
			  m_offset{other.m_offset},
			  m_length{other.m_length},
			  m_element_size{other.m_element_size},
			  m_map_size{other.m_map_size},
			  m_free_list{std::move(other.m_free_list)}
		{
			other.m_memory = nullptr;
			other.m_offset = 0;
			other.m_length = 0;
		}

		MemPool& operator=(const MemPool& other)
		{
			if (this == &other)
			{
				return *this;
			}

			m_memory       = other.m_memory;
			m_offset       = other.m_offset;
			m_length       = other.m_length;
			m_element_size = other.m_element_size;
			m_map_size     = other.m_map_size;
			m_free_list    = other.m_free_list;

			return *this;
		}

		MemPool& operator=(MemPool&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			m_memory       = other.m_memory;
			m_offset       = other.m_offset;
			m_length       = other.m_length;
			m_element_size = other.m_element_size;
			m_map_size     = other.m_map_size;
			m_free_list    = std::move(other.m_free_list);

			return *this;
		}

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
			auto [mem, offset] = allocate();

			return {new (mem) T(std::forward<Args>(args)...), offset};
		}

		template<class T>
		std::pair<T*, size_t> allocate(T &&data)
		{
			auto [mem, offset] = allocate();

			return {new (mem) T(std::forward<T>(data)), offset};
		}

		void free(size_t offset_to_free, bool destroy = true);

		void reset(bool destroy = true);

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
}
