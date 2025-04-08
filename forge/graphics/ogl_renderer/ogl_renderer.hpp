#pragma once
#include <cstdint>
#include <functional>

#include "ogl_buffers.hpp"
#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "render_resource.hpp"
#include "forge/concurrency/command_buffer.hpp"
#include "forge/core/isub_system.hpp"
#include "forge/graphics/camera.hpp"
#include "forge/graphics/lights.hpp"
#include "forge/graphics/material.hpp"
#include "forge/memory/mem_pool.hpp"

class GLFWwindow;

#define OGL_MAX_LIGHTS 8

namespace forge
{
	struct OglRendererArgConfig
	{
		std::string_view shader_path = FORGE_ENGINE_ASSET_DIR "shaders/";
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

		// allows for manually adding a command that will be run the next frame
		void add_command(CommandBuffer<>::Callback &&command)
		{
			m_command_buffer.emplace(std::forward<CommandBuffer<>::Callback>(command));
		}

		Light* create_light();

		void destroy_light(Light *light);

	private:
		bool m_draw_wireframe = false;
		OglBuffers m_cube_buffers;
		OglShader m_forward_shader;
		CommandBuffer<> m_command_buffer;
		RenderStatistics m_statistics;
		RenderResource<OglTexture> m_texture_resource;

		OglRendererArgConfig m_arg_config;

		std::array<Light, OGL_MAX_LIGHTS> m_lights {};

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
