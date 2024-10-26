#pragma once

#ifdef __linux__
	#define AGNOSTIC_INCLUDE
	#include "linux/linux_system_info.hpp"
#endif