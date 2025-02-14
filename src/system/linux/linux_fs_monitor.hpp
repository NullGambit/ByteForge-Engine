#pragma once

#include <cstdint>
#include <functional>
#include <string_view>
#include <string>
#include <sys/inotify.h>

#include "core/isub_system.hpp"

namespace forge
{
	// TODO add more events
	enum LINUX_FS_EVENT : uint32_t
	{
		LINUX_FSE_MODIFY = IN_MODIFY,
		LINUX_FSE_ACCESS = IN_ACCESS,
		LINUX_FSE_CREATE = IN_CREATE,
		LINUX_FSE_DELETE = IN_DELETE_SELF,
		LINUX_FSE_FILE_CREATE = IN_CREATE,
		LINUX_FSE_FILE_DELETE = IN_DELETE,
	};

	class LinuxFsMonitor final : public ISubSystem
	{
	public:
		using Callback = std::function<void(uint32_t, std::string_view path)>;

		std::string init() override;
		void shutdown() override;
		void update() override;

		int add_watch(std::string_view path, uint32_t events, Callback callback);
		bool remove_watch(int wd);

		uint32_t poll();

	private:

		std::mutex m_mutex;

		int m_fd;

		struct Watchers
		{
			uint32_t events;
			Callback callback;
			bool is_alive = true;
		};

		std::vector<Watchers> m_watchers;
		std::string m_event_buffer;
	};
}
