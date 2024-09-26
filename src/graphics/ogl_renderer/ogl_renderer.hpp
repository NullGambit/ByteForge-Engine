#pragma once
#include <cstdint>
#include <functional>

#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "concurrency/command_buffer.hpp"
#include "core/isub_system.hpp"

namespace forge
{
	class OglRenderer : public ISubSystem
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

		void toggle_wireframe();

		// allows for manually adding a command that will be run the next frame
		void add_command(CommandBuffer<>::Callback &&command)
		{
			m_command_buffer.emplace(std::forward<CommandBuffer<>::Callback>(command));
		}

	private:
		bool m_draw_wireframe = false;
		uint32_t m_vao;
		OglShader m_tri_shader;
		OglTexture m_wall_texture;
		OglTexture m_face_texture;
		CommandBuffer<> m_command_buffer;

		void handle_framebuffer_resize(int width, int height);
	};

}
