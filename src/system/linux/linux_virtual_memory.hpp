#pragma once

#include <cstddef>

namespace forge
{
	u8* linux_virtual_alloc(size_t size);
	void linux_virtual_free(u8 *ptr);
}