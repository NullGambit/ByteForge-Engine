#pragma once

#if __linux__
#include "linux/linux_virtual_memory.hpp"
namespace forge
{
	constexpr auto virtual_alloc = linux_virtual_alloc;
	constexpr auto virtual_free = linux_virtual_free;
}
#else
#error unsupported platform for virtual memory
#endif