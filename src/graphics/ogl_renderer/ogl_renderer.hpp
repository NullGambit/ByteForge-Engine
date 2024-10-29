#pragma once
#include <cstdint>
#include <functional>

#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "concurrency/command_buffer.hpp"
#include "core/isub_system.hpp"
#include "graphics/camera.hpp"
#include "graphics/material.hpp"

class GLFWwindow;

namespace forge
{
	struct RenderStatistics
	{
		u32 draw_calls;
	};

	struct PrimitiveModel
	{
		glm::mat4 model;
		Material material;
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

		u32 create_primitive(PrimitiveModel primitive);

		void update_primitive(u32 id, PrimitiveModel primitive);

		void update_primitive_material(u32 id, Material material);

		void destroy_primitive(u32 id);

		void primitive_set_hidden(u32 id, bool value);

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
		uint32_t m_cube_vao;
		OglShader m_forward_shader;
		OglTexture m_cube_texture;
		OglTexture m_face_texture;
		GLFWwindow *m_worker_window = nullptr;
		CommandBuffer<> m_command_buffer;
		RenderStatistics m_statistics;

		struct PrimitiveRenderData
		{
			PrimitiveModel primitive;
			// if false this entry is freed
			bool is_valid = true;
			// if false this primitive should not be drawn
			bool is_hidden = false;
		};

		std::vector<PrimitiveRenderData> m_cube_positions;
		glm::mat4 m_pv;

		void handle_framebuffer_resize(int width, int height);

		u32 get_free_rdi();
	};

}
