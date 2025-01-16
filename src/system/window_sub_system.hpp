#pragma once
#include "window.hpp"
#include "../core/isub_system.hpp"
#include "container/virtual_array.hpp"


#define MAX_WINDOW_COUNT 32

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

	private:
		VirtualArray<Window> m_windows {MAX_WINDOW_COUNT};
	};
}
