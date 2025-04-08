#pragma once
#include "forge/core/logging.hpp"

namespace forge
{
	template<class T>
	class RenderResource
	{
	public:

		typedef void(*OnResourceInit)(std::string_view, T*);
		typedef void(*OnResourceDeInit)(T*);

		OnResourceInit on_resource_init;
		OnResourceDeInit on_resource_deinit;

		T get(std::string_view id)
		{
			auto iter = m_resources.find(id);

			if (iter == m_resources.end())
			{
				return nullptr;
			}

			return &iter->second.first;
		}

		T add(std::string_view id)
		{
			auto iter = m_resources.find(id);

			if (iter == m_resources.end())
			{
				iter = m_resources.emplace(id, std::make_pair(T{}, 0)).first;

				if (on_resource_init)
				{
					on_resource_init(id, &iter->second.first);
				}
			}

			iter->second.second++;

			return iter->second.first;
		}

		void remove(std::string_view id)
		{
			if (id.empty())
			{
				return;
			}

			auto iter = m_resources.find(id);

			if (iter == m_resources.end())
			{
				return;
			}

			iter->second.second--;

			if (iter->second.second <= 0)
			{
				m_resources.erase(iter);
			}
		}

	private:
		HashMap<std::string, std::pair<T, i32>, ENABLE_TRANSPARENT_HASH> m_resources;
	};
}
