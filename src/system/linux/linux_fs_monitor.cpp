
#include "linux_fs_monitor.hpp"

#include <sys/inotify.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <utility>
#include "linux_fs_monitor.hpp"

#include "fmt/fmt.hpp"

#define INITIAL_MONITOR_SIZE 50

std::string forge::LinuxFsMonitor::init()
{
	m_fd = inotify_init1(IN_NONBLOCK);

	if (m_fd == -1)
	{
		return "could not init inotify";
	}

	m_watchers.resize(INITIAL_MONITOR_SIZE);

	return {};
}

void forge::LinuxFsMonitor::shutdown()
{
	close(m_fd);
}

void forge::LinuxFsMonitor::update()
{
	poll();
}

int forge::LinuxFsMonitor::add_watch(std::string_view path, uint32_t events, Callback callback)
{
	std::lock_guard lock {m_mutex};

	int wd = inotify_add_watch(m_fd, path.data(), events);

	if (wd == -1)
	{
		return -1;
	}

	if (wd >= m_watchers.size())
	{
		m_watchers.resize(m_watchers.size() * 2);
	}

	m_watchers[wd] =
	{
		events,
		std::move(callback)
	};

	return wd;
}

bool forge::LinuxFsMonitor::remove_watch(int wd)
{
	std::lock_guard lock {m_mutex};

	return inotify_rm_watch(m_fd, wd) == 0;
}

uint32_t forge::LinuxFsMonitor::poll()
{
	uint32_t bytes_to_read;

	ioctl(m_fd, FIONREAD, &bytes_to_read);

	if (bytes_to_read <= 0)
	{
		return 0;
	}

	if (bytes_to_read >= m_event_buffer.size())
	{
		m_event_buffer.resize(bytes_to_read);
	}

	auto bytes_read = read(m_fd, m_event_buffer.data(), bytes_to_read);
	int bytes_processed = 0;

	std::lock_guard lock {m_mutex};

	while (bytes_processed < bytes_read)
	{
		auto event = (inotify_event*)(m_event_buffer.data() + bytes_processed);
		auto watcher = m_watchers[event->wd];

		if (event->mask & watcher.events)
		{
			watcher.callback(event->mask, std::string_view{event->name, event->len});
		}

		bytes_processed += sizeof(inotify_event) + event->len;
	}

	return bytes_processed / sizeof(inotify_event);
}
