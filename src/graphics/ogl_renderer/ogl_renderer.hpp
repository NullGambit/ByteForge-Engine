#pragma once
#include <cstdint>
#include <functional>

#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "concurrency/command_buffer.hpp"
#include "core/isub_system.hpp"
#include "graphics/camera.hpp"

class GLFWwindow;

namespace forge
{
	struct RenderStatistics
	{
		u32 draw_calls;
	};

	class OglRenderer final : public ISubSystem
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

		void start_tick() override;

		void set_wireframe(bool enable);

		void set_clear_color(glm::vec3 color);

		glm::vec3 get_clear_color();

		[[nodiscard]]
		RenderStatistics get_statistics() const
		{
			return m_statistics;
		}

		// allows for manually adding a command that will be run the next frame
		void add_command(CommandBuffer<>::Callback &&command)
		{
			m_command_buffer.emplace(std::forward<CommandBuffer<>::Callback>(command));
		}

		inline void update_pv(const glm::mat4 &pv)
		{
			m_command_buffer.emplace([pv, &this_pv = m_pv]
			{
				this_pv = pv;
			});
		}

	private:
		bool m_draw_wireframe = false;
		uint32_t m_vao;
		OglShader m_tri_shader;
		OglTexture m_cube_texture;
		OglTexture m_face_texture;
		GLFWwindow *m_worker_window = nullptr;
		CommandBuffer<> m_command_buffer;
		RenderStatistics m_statistics;

		struct Transform
		{
			glm::vec3 position;
			glm::vec3 euler_angle;
		};

		std::vector<Transform> m_cube_positions;
		glm::mat4 m_pv;

		void handle_framebuffer_resize(int width, int height);
	};

}
