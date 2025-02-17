#pragma once

inline size_t align_to(size_t value, size_t alignment)
{
	return value + (alignment - 1) & ~(alignment - 1);
}

