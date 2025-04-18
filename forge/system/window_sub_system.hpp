#pragma once

#include "window.hpp"
#include "forge/core/isub_system.hpp"
#include "forge/container/virtual_array.hpp"

#define MAX_WINDOW_COUNT 32

inline forge::Window *g_main_window;

namespace forge
{
	class WindowSubSystem final : public ISubSystem
	{
	public:
		std::string init(const EngineInitOptions &options) override;
		void shutdown() override;
		void update() override;
		bool is_critical() override { return true; }
		void pre_update() override;
		void post_update() override;

		// set to highest update order so it will be able to swap buffers after everything is done rendering
		u32 get_update_order() override
		{
			return UINT32_MAX;
		}

		Window *create_window();
		void destroy_window(Window *window);

		Window* get_main_window();

		[[nodiscard]]
		float get_runtime() const;

	private:
		VirtualArray<Window> m_windows;
	};
}
