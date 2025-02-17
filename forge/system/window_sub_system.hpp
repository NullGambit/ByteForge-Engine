#pragma once

#include "window.hpp"
#include "forge/core/isub_system.hpp"
#include "forge/container/virtual_array.hpp"

#define MAX_WINDOW_COUNT 32

namespace forge
{
	class WindowSubSystem final : public ISubSystem
	{
	public:
		std::string init(const EngineInitOptions &options) override;
		void shutdown() override;
		void update() override;
		bool is_critical() override { return true; }
		void post_update() override;

		void poll_events();

		[[nodiscard]]
		float get_runtime() const;

	private:
		VirtualArray<Window> m_windows {MAX_WINDOW_COUNT};
	};
}
