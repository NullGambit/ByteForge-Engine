#pragma once
#include <string_view>

namespace forge
{
	using IdType = u64;

	class StringId
	{
	public:
		StringId(std::string_view string);

		bool operator ==(const StringId &other) const
		{
			return m_id == other.m_id;
		}

		bool operator !=(const StringId &other) const
		{
			return m_id != other.m_id;
		}

		[[nodiscard]]
		auto get_string() const
		{
			return m_string;
		}

		[[nodiscard]]
		auto get_id() const
		{
			return m_id;
		}

		[[nodiscard]]
		auto begin() const
		{
			return m_string.begin();
		}

		[[nodiscard]]
		auto end() const
		{
			return m_string.end();
		}

	private:
		IdType m_id;
		std::string_view m_string;
	};
}
