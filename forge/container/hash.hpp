#pragma once

#include "ankerl/unordered_dense.h"

namespace forge
{
	template<class Key>
	using Hash = ankerl::unordered_dense::hash<Key>;
}