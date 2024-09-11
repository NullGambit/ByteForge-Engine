#pragma once
#include "../core/isub_system.hpp"

namespace forge
{
	class WindowSubSystem : public ISubSystem
	{
	public:
		void init() override;
		void shutdown() override;
		void update() override;

		bool is_ok() const override;
		std::string_view error_message() const override;

	private:
		bool m_ok = true;
		std::string_view m_error;
	};
}
