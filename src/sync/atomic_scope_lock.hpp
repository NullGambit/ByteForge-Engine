#pragma once
#include <atomic>

namespace forge
{
	class AtomicScopeLock
	{
	public:

		AtomicScopeLock(std::atomic_bool &flag) :
			m_flag(flag)
		{
			m_flag = true;
		}

		~AtomicScopeLock()
		{
			m_flag = false;
		}

	private:
		std::atomic_bool &m_flag;
	};
}
