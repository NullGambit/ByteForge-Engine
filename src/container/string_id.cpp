#include "string_id.hpp"

#include "map.hpp"
#include <shared_mutex>

static std::shared_mutex g_string_id_mutex;
static forge::HashMap<forge::IdType, std::string> g_string_id_table;

forge::StringId::StringId(std::string_view string)
{
	constexpr Hash<std::string_view> hasher;

	auto hash = hasher(string);

	g_string_id_mutex.lock_shared();

	auto iter = g_string_id_table.find(hash);

	g_string_id_mutex.unlock_shared();

	if (iter == g_string_id_table.end())
	{
		std::lock_guard lock {g_string_id_mutex};
		iter = g_string_id_table.emplace(hash, string).first;
	}

	m_id = hash;
	m_string = iter->second;
}
