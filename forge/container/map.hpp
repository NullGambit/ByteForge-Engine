#pragma once

#include "hash.hpp"
#include "ankerl/unordered_dense.h"

#define ENABLE_TRANSPARENT_HASH forge::string_hash, std::equal_to<>

namespace forge
{
	template <class Key,
			  class T,
			  class Hash = Hash<Key>,
			  class KeyEqual = std::equal_to<Key>,
			  class AllocatorOrContainer = std::allocator<std::pair<Key, T>>,
			  class Bucket = ankerl::unordered_dense::bucket_type::standard>
    using HashMap = ankerl::unordered_dense::map<Key, T, Hash, KeyEqual, AllocatorOrContainer, Bucket>;

	struct string_hash
    {
        using hash_type = Hash<std::string_view>;

        using is_transparent = void;
		using is_avalanching = void;

        size_t operator()(const char *str) const { return hash_type{}(str); }
        size_t operator()(std::string_view str) const { return hash_type{}(str); }
        size_t operator()(std::string const &str) const { return hash_type{}(str); }
    };
}