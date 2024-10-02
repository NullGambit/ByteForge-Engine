#pragma once
#include "core/isub_system.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	class ImGuiSubsystem final : public ISubSystem
	{
	public:
		std::string init() override;
		void shutdown() override;
		void update() override;
		void start_tick() override;
		void end_tick() override;

		// SubSystemThreadMode get_thread_mode() override
		// {
		// 	return SubSystemThreadMode::OffloadThread;
		// }
	};
}
