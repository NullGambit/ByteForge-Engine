#define AGNOSTIC_INCLUDE
#include "linux_system_info.hpp"

#include <unistd.h>

size_t get_page_size()
{
	return sysconf(_SC_PAGESIZE);
}
