#pragma once
#include "../core/isub_system.hpp"

class Window;

namespace forge
{
	class WindowSubSystem final : public ISubSystem
	{
	public:
		std::string init() override;
		void shutdown() override;
		void update() override;
		bool is_critical() override { return true; }
		void end_tick() override;

		void poll_events();

		[[nodiscard]]
		float get_runtime() const;

	};
}
