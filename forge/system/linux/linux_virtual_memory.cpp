#include "linux_virtual_memory.hpp"

#include <sys/mman.h>
#include <cstring>

#include "../system_info.hpp"
#include "forge/memory/mem_utils.hpp"

u8* forge::linux_virtual_alloc(size_t size)
{
	constexpr auto n = sizeof(size_t);

	size = align_to(size + n, get_page_size());

	auto *ptr = (u8*)mmap(nullptr, size + n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (ptr == (void *) -1)
	{
		return nullptr;
	}

	memcpy(ptr, &size, n);

	return ptr + n;
}

void forge::linux_virtual_free(u8* ptr)
{
	if (!ptr)
	{
		return;
	}

	auto size = (size_t)(ptr - sizeof(size_t));

	munmap(ptr, size);
}
