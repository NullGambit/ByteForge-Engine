#include "arg_parser.hpp"

#include <iostream>

#include "forge/core/logging.hpp"
#include "forge/util/types.hpp"

std::string forge::ArgParser::parse(std::span<const char*> args)
{
	for (auto i = 0; i < args.size(); i++)
	{
		std::string_view arg = args[i];

		if (arg == "--help" || arg == "-h")
		{
			output_help();
			return "h";
		}

		auto starts_with_prefix = arg.starts_with(m_prefix);
		auto starts_with_alias  = arg.starts_with(m_alias_prefix);

		if (starts_with_prefix || starts_with_alias)
		{
			auto prefix_length = starts_with_prefix ? m_prefix.size() : m_alias_prefix.size();

			auto name = arg.substr(prefix_length);

			auto opt = find_arg_value(name);

			if (!opt && m_strict)
			{
				auto partial_match = find_partial_match(name);
				return fmt::format("no flag named {} {}", name,
					partial_match.empty() ? "" : fmt::format("did you mean {}", partial_match));
			}

			auto storage = opt.value();

			std::string_view value;

			if (storage.index() != ArgValueType::Bool)
			{
				i++;

				if (i >= args.size())
				{
					return fmt::format("expected value for flag {} but reached end of arguments", name);
				}

				value = args[i];

				if (value.starts_with(m_prefix) || value.starts_with(m_alias_prefix))
				{
					return fmt::format("expected value for flag {} but got another flag instead", name);
				}
			}

			auto result = std::visit(util::overload
			{
				[](bool *ptr)
				{
					*ptr = true;
					return true;
				},
				[&value](int *ptr)
				{
					auto result = std::from_chars(value.begin(), value.end(), *ptr);
					return result.ec != std::errc::invalid_argument;
				},
				[&value](float *ptr)
				{
					auto result = std::from_chars(value.begin(), value.end(), *ptr);
					return result.ec != std::errc::invalid_argument;
				},
				[&value](std::string_view *ptr)
				{
					*ptr = value;
					return true;
				},
			}, storage);

			if (!result)
			{
				return fmt::format("unable to parse the value for flag {}", name);
			}
		}
		else
		{
			m_positional_args.emplace_back(arg);
		}
	}

	return "";
}

std::optional<forge::ArgValue> forge::ArgParser::find_arg_value(std::string_view name, bool use_alias_table)
{
	auto &table = use_alias_table ? m_alias_table : m_args;
	auto iter = table.find(name);

	if (iter == table.end())
	{
		if (use_alias_table)
		{
			return std::nullopt;
		}

		return find_arg_value(name, true);
	}

	return iter->second.value;
}

std::string_view forge::ArgParser::find_partial_match(std::string_view name)
{
	std::pair<std::string_view, u32> highest_match {};

	for (auto &[flag, _] : m_args)
	{
		auto matches = 0;
		auto len = std::min(name.size(), flag.size());

		for (auto i = 0; i < len; i++)
		{
			if (name[i] == flag[i])
			{
				matches++;
			}
		}

		if (matches >= highest_match.second)
		{
			highest_match = {flag, matches};
		}
	}

	return highest_match.first;
}

void forge::ArgParser::output_help() const
{
	HashMap<std::string_view, std::vector<std::string_view>> groups;

	for (auto &[flag, data] : m_args)
	{
		auto iter = groups.find(data.meta.group);

		if (iter == groups.end())
		{
			iter = groups.emplace(data.meta.group, std::vector<std::string_view>{}).first;
		}

		iter->second.emplace_back(flag);
	}

	for (auto &[group, flag_names] : groups)
	{
		if (group.empty())
		{
			group = "general";
		}

		std::cout << "\n\n======== " << group << " ========\n\n";

		for (auto &flag : flag_names)
		{
			auto &data = m_args.find(flag)->second;

			std::cout << m_prefix << flag;

			if (!data.meta.alias.empty())
			{
				std::cout << " " << m_alias_prefix << data.meta.alias;
			}

			std::cout << " <" << get_arg_value_string(data.value.index()) << ">";

			if (!data.meta.description.empty())
			{
				std::cout << " " << data.meta.description;
			}

			std::cout << '\n';
		}
	}
}
