#pragma once
#include "forge/system/virtual_memory.hpp"

namespace forge
{
	template<class T>
	struct VirtualAllocator
	{
		using value_type = T;

		VirtualAllocator() = default;

		template<class U>
		constexpr VirtualAllocator(const VirtualAllocator <U>&) noexcept {}

		static T* allocate(size_t n)
		{
			return (T*)virtual_alloc(n);
		}

		static u8* allocate_bytes(size_t n)
		{
			return virtual_alloc(n);
		}

		static void deallocate(T* p, std::size_t n) noexcept
		{
			virtual_free((u8*)p);
		}

		static void deallocate_bytes(u8* p, std::size_t n) noexcept
		{
			virtual_free(p);
		}
	};
}
