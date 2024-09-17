#pragma once

#if __linux__
#include "linux/linux_fs_monitor.hpp"
	namespace forge
	{
		enum FS_EVENT : uint32_t
		{
			FSE_MODIFY 		= LINUX_FSE_MODIFY,
			FSE_ACCESS 		= LINUX_FSE_ACCESS,
			FSE_CREATE 		= LINUX_FSE_CREATE,
			FSE_DELETE		= LINUX_FSE_DELETE,
			FSE_FILE_CREATE = LINUX_FSE_FILE_CREATE,
			FSE_FILE_DELETE = LINUX_FSE_FILE_DELETE,
		};

		using FsMonitor = LinuxFsMonitor;
	}
#else
#error unsupported platform for fs monitor
#endif