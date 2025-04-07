#pragma once
#include <vector>

namespace forge
{
	template<class T, class Allocator = std::allocator<T> >
	using Array = std::vector<T, Allocator>;
}
