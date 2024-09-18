#pragma once
#include <cstdint>

#include "ogl_shader.hpp"
#include "../../core/isub_system.hpp"

namespace forge
{
	class OglRenderSubSystem : public ISubSystem
	{
	public:
		std::string init() override;
		void update() override;
		void shutdown() override;
		bool is_critical() override { return true; }

		// SubSystemThreadMode get_thread_mode() override
		// {
		// 	return SubSystemThreadMode::OffloadThread;
		// }

	private:

		uint32_t m_vao;
		OglShader m_tri_shader;

		void handle_framebuffer_resize(int width, int height);
	};

}
