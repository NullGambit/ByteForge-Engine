#pragma once

#include <string>

#include "forge/core/isub_system.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	class ImGuiSubsystem final : public ISubSystem
	{
	public:
		std::string init(const EngineInitOptions &options) override;
		void shutdown() override;
		void update() override;
		void pre_update() override;
		void post_update() override;

		// SubSystemThreadMode get_thread_mode() override
		// {
		// 	return SubSystemThreadMode::OffloadThread;
		// }
	};
}
