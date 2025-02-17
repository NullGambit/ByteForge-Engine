#pragma once

#include "ankerl/unordered_dense.h"
#include "hash.hpp"

namespace forge
{
	template <class Key,
			  class Hash = Hash<Key>,
			  class KeyEqual = std::equal_to<Key>,
			  class AllocatorOrContainer = std::allocator<Key>,
			  class Bucket = ankerl::unordered_dense::bucket_type::standard>
	using HashSet = ankerl::unordered_dense::set<Key, void, Hash, KeyEqual, AllocatorOrContainer, Bucket, false>;
}