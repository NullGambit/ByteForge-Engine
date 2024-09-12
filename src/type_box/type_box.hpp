#pragma once
#include <atomic>
#include <mutex>
#include <typeindex>

#include "container/map.hpp"
#include "memory/mem_pool.hpp"
#include "memory/defs.hpp"
#include "sync/atomic_scope_lock.hpp"

#define TYPE_BOX_MAX_MEM GB(2)

namespace forge
{
	struct TypeBoxData
	{
		MemPool mem_pool;
	};

	template<class T>
	class TypeFunnel
	{
	public:

		TypeFunnel(TypeBoxData &data) :
			m_data(data)
		{}

		inline size_t size() const
		{
			return m_data.mem_pool.length();
		}

		T* next()
		{
			if (m_offset >= m_data.mem_pool.offset())
			{
				return nullptr;
			}

			auto *ptr = m_data.mem_pool.memory() + m_offset;

			m_offset += sizeof(T);

			return (T*)ptr;
		}

	private:
		size_t m_offset = 0;
		TypeBoxData &m_data;
	};

	class TypeBox
	{
	public:

		~TypeBox()
		{
			clear_all();
		}

		template<class T>
		void put(T &&data)
		{
			std::lock_guard lock { m_mutex };

			auto &type_data = get_or_emplace<T>();

			type_data.mem_pool.allocate(std::forward<T>(data));
		}

		template<class T, class ...Args>
		void emplace(Args &&...args)
		{
			std::lock_guard lock { m_mutex };

			auto &type_data = get_or_emplace<T>();

			type_data.mem_pool.template emplace<T>(std::forward<Args>(args)...);
		}

		template<class T>
		std::optional<TypeFunnel<T>> fetch(bool wait = true)
		{
			if (wait)
			{
				m_mutex.lock();
			}
			else
			{
				if (!m_mutex.try_lock())
				{
					return std::nullopt;
				}
			}

			std::lock_guard lock (m_mutex, std::adopt_lock);

			auto iter = m_type_container.find(typeid(T));

			if (iter == m_type_container.end())
			{
				return std::nullopt;
			}

			return iter->second;
		}

		void clear_temp()
		{

		}

		template<class T>
		void clear()
		{
			clear_implementation(typeid(T));
		}

		void clear_all()
		{
			for (auto &[index, type_data] : m_type_container)
			{
				type_data.mem_pool.reset();
			}
		}

	private:
		std::mutex m_mutex;
		HashMap<std::type_index, TypeBoxData> m_type_container;

		template<class T>
		TypeBoxData& get_or_emplace()
		{
			auto iter = m_type_container.find(typeid(T));

			if (iter == m_type_container.end())
			{
				iter = m_type_container.emplace(typeid(T), TypeBoxData{}).first;
				iter->second.mem_pool.init<T>(TYPE_BOX_MAX_MEM);
			}

			return iter->second;
		}

		void clear_implementation(std::type_index index)
		{
			auto iter = m_type_container.find(index);

			if (iter == m_type_container.end())
			{
				return;
			}

			iter->second.mem_pool.reset();
		}
	};
}
