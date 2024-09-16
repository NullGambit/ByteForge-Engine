#pragma once

#if __linux__
#include "linux/linux_fs_monitor.hpp"
	namespace forge
	{
		using FS_EVENTS = LINUX_FS_EVENT;
		using FsMonitor = LinuxFsMonitor;
	}
#else
#error unsupported platform for fs monitor
#endif