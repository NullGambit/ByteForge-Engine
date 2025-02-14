#pragma once
#include <cstdint>
#include <functional>

#include "gl_buffers.hpp"
#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "render_resource.hpp"
#include "concurrency/command_buffer.hpp"
#include "core/isub_system.hpp"
#include "graphics/camera.hpp"
#include "graphics/material.hpp"
#include "memory/mem_pool.hpp"

class GLFWwindow;

namespace forge
{
	struct OglRendererArgConfig
	{
		std::string_view shader_path = "./assets/shaders/";
	};

	struct RenderStatistics
	{
		u32 draw_calls;
	};

	class OglRenderer;

	struct PrimitiveModel
	{
		Material material;

		// if false this primitive should not be drawn
		bool is_hidden = false;

		PrimitiveModel() = default;

		PrimitiveModel(glm::mat4 model, Material material = {}) :
			material(material)
		{
			update_model(model);
		}

		void update_model(const glm::mat4 &model)
		{
			m_model = model;
			m_normal_matrix = glm::transpose(glm::inverse(model));
		}

		u32 get_id() const
		{
			return m_id;
		}

	private:
		friend OglRenderer;

		glm::mat4 m_normal_matrix;
		glm::mat4 m_model;
		u32 m_id;
	};

	class OglRenderer final : public ISubSystem
	{
	public:
		std::string init(const EngineInitOptions &options) override;
		void update() override;
		void shutdown() override;
		bool is_critical() override { return true; }

		std::vector<DependencyStorage> get_dependencies() override;

		void receive_cmd_args(ArgParser &parser) override;

		void pre_update() override;

		void set_wireframe(bool enable);

		void set_clear_color(glm::vec3 color);

		glm::vec3 get_clear_color();

		[[nodiscard]]
		RenderStatistics get_statistics() const
		{
			return m_statistics;
		}

		void set_active_camera(Camera *camera);

		Camera* get_active_camera();

		PrimitiveModel* create_primitive(glm::mat4 model = {});

		void destroy_primitive(u32 id);

		void destroy_texture(std::string_view path);

		void create_texture(u32 id, std::string_view path, u32 type);

		void update_light(glm::vec3 position, glm::vec3 color)
		{
			m_light_color = color;
			m_light_position = position;
		}

		// allows for manually adding a command that will be run the next frame
		void add_command(CommandBuffer<>::Callback &&command)
		{
			m_command_buffer.emplace(std::forward<CommandBuffer<>::Callback>(command));
		}

	private:
		bool m_draw_wireframe = false;
		GlBuffers m_cube_buffers;
		OglShader m_forward_shader;
		GLFWwindow *m_worker_window = nullptr;
		CommandBuffer<> m_command_buffer;
		RenderStatistics m_statistics;
		RenderResource<OglTexture> m_texture_resource;
		glm::vec3 m_light_position {};
		glm::vec3 m_light_color {1.0};

		OglRendererArgConfig m_arg_config;

		Camera m_default_camera;
		Camera *m_active_camera;

		MemPool m_render_data_pool;

		struct TextureData
		{
			OglTexture texture;
			std::string_view path;
			bool is_valid = false;
		};

		struct PrimitiveRenderData
		{
			PrimitiveModel primitive;
			// if false this entry is freed
			bool is_valid = true;

			TextureList<TextureData> textures;
		};

		void handle_framebuffer_resize(int width, int height);
	};

}
