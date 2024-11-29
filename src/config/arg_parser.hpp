#pragma once
#include <span>
#include <variant>

#include "container/map.hpp"

namespace forge
{
	namespace ArgValueType
	{
		enum
		{
			Int,
			Float,
			String,
			Bool,
		};
	}

	using ArgValue = std::variant<i32*, f32*, std::string_view*, bool*>;

	struct ArgMeta
	{
		std::string_view alias;
		std::string_view description;
	};

	class ArgParser
	{
		struct ArgData
		{
			ArgValue value;
			ArgMeta meta;
		};

	public:

		ArgParser(std::string_view prefix = "--", std::string_view alias_prefix = "-", bool strict = true) :
			m_strict(strict),
			m_prefix(prefix),
			m_alias_prefix(alias_prefix)
		{}

		template<class T>
		requires(std::is_constructible_v<ArgValue, T*>)
		ArgParser& add(std::string_view name, T *storage, ArgMeta meta = {})
		{
			ArgData data
			{
				storage,
				meta
			};

			m_args.emplace(name, data);

			if (!meta.alias.empty())
			{
				m_args.emplace(meta.alias, data);
			}

			return *this;
		}

		std::string parse(std::span<const char*> args);

		[[nodiscard]]
		inline std::string_view get_positional(size_t index) const
		{
			return m_positional_args[index];
		}

		[[nodiscard]]
		inline size_t get_positional_size() const
		{
			return m_positional_args.size();
		}

	private:
		bool m_strict;
		std::string_view m_prefix;
		std::string_view m_alias_prefix;
		HashMap<std::string_view, ArgData> m_args;
		// the alias table holds a copy of the arg data. this should be ok as all of arg datas members are trivially copyable
		// and holding a pointer to an item inside HashMap will become invalid
		HashMap<std::string_view, ArgData> m_alias_table;
		std::vector<std::string_view> m_positional_args;

		std::optional<ArgValue> find_arg_value(std::string_view name, bool use_alias_table = false);

		std::string_view find_partial_match(std::string_view name);
	};
}
