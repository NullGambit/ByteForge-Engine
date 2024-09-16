#pragma once

#include <cstdint>
#include <functional>
#include <string_view>
#include <string>
#include <sys/inotify.h>

namespace forge
{
	// TODO add more events
	enum LINUX_FS_EVENT : uint32_t
	{
		FSE_MODIFY = IN_MODIFY,
		FSE_ACCESS = IN_ACCESS,
		FSE_CREATE = IN_CREATE,
		FSE_DELETE = IN_DELETE_SELF,
		FSE_FILE_CREATE = IN_CREATE,
		FSE_FILE_DELETE = IN_DELETE,
	};

	class LinuxFsMonitor
	{
	public:
		using Callback = std::function<void(uint32_t, std::string_view path)>;

		LinuxFsMonitor();
		~LinuxFsMonitor();

		int add_watch(std::string_view path, uint32_t events, Callback callback);
		bool remove_watch(int wd) const;
		uint32_t poll();

	private:
		int m_fd;

		struct Monitors
		{
			uint32_t events;
			Callback callback;
		};

		std::vector<Monitors> m_monitors;
		std::string m_event_buffer;
	};
}
