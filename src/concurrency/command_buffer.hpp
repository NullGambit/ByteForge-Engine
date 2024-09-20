#pragma once
#include <functional>

namespace forge
{
	template<class ...Args>
	class CommandBuffer
	{
	public:
		using Callback = std::function<void(Args...)>;

		void emplace(Callback &&command)
		{
			std::lock_guard lock {m_mutex};

			auto &buffer = get_active_buffer();

			buffer.emplace_back(std::forward<Callback>(command));
		}

		void execute_all(Args &&...args)
		{
			m_mutex.lock();

			auto &buffer = get_active_buffer();

			m_use_buffer_a = !m_use_buffer_a;

			m_mutex.unlock();

			if (buffer.empty())
			{
				return;
			}

			for (auto &command : buffer)
			{
				command(std::forward<Args>(args)...);
			}

			buffer.clear();
		}

	private:
		std::mutex m_mutex;
		// TODO: its probably a good idea to use an object pool for the std::function objects
		std::vector<Callback> m_buffer_a;
		std::vector<Callback> m_buffer_b;
		bool m_use_buffer_a = true;

		inline std::vector<Callback>& get_active_buffer()
		{
			return m_use_buffer_a ? m_buffer_a : m_buffer_b;
		}
	};
}
