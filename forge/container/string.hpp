#pragma once

#include <string>
#include <string_view>

namespace forge
{
	using String = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;

	template<class Allocator>
	using StringA = std::basic_string<char, std::char_traits<char>, Allocator>;

	using StringView = std::string_view;
}