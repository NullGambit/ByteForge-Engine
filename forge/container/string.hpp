#pragma once
#include <string>

namespace forge
{
	using String = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;

	template<class Allocator>
	using StringA = std::basic_string<char, std::char_traits<char>, Allocator>;
}