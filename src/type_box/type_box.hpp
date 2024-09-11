#pragma once
#include <atomic>
#include <typeindex>

#include "container/map.hpp"
#include "memory/mem_pool.hpp"
#include "memory/defs.hpp"

#define TYPE_BOX_MAX_MEM GB(2)

namespace forge
{
	struct TypeBoxData
	{
		MemPool mem_pool;
		std::unique_ptr<std::atomic_flag> is_locked = std::make_unique<std::atomic_flag>();
	};

	template<class T>
	class TypeFunnel
	{
	public:

		TypeFunnel(TypeBoxData &data) :
			m_data(data)
		{
			m_data.is_locked->test_and_set(std::memory_order_acquire);
		}

		~TypeFunnel()
		{
			m_data.is_locked->clear(std::memory_order_release);
		}

		inline size_t size() const
		{
			return m_data.mem_pool.length();
		}

		T* next()
		{
			if (m_offset > size())
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
			auto &type_data = get_or_emplace<T>();

			type_data.mem_pool.allocate(std::forward<T>(data));
		}

		template<class T, class ...Args>
		void emplace(Args &&...args)
		{
			auto &type_data = get_or_emplace<T>();

			type_data.mem_pool.template emplace<T>(std::forward<Args>(args)...);
		}

		template<class T>
		std::optional<TypeFunnel<T>> fetch()
		{
			auto iter = m_type_container.find(typeid(T));

			if (iter == m_type_container.end())
			{
				return std::nullopt;
			}

			if (iter->second.is_locked->test(std::memory_order_relaxed))
			{
				return std::nullopt;
			}

			return iter->second;
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

			m_type_container.clear();
		}
	private:
		HashMap<std::type_index, TypeBoxData> m_type_container;

		template<class T>
		TypeBoxData& get_or_emplace()
		{
			auto iter = m_type_container.find(typeid(T));

			if (iter == m_type_container.end())
			{
				iter = m_type_container.emplace(typeid(T), TypeBoxData{}).first;
				iter->second.mem_pool.init(sizeof(T), TYPE_BOX_MAX_MEM);
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
