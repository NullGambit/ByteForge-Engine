#pragma once
#pragma once
#include <string>

namespace forge
{
	template<class Allocator = std::allocator<char>>
	using String = std::string<char, std::char_traits<char>, Allocator>;
}