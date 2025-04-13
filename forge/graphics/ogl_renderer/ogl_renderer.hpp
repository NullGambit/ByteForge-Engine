#pragma once
#include <cstdint>
#include <functional>

#include "ogl_buffers.hpp"
#include "ogl_shader.hpp"
#include "ogl_texture.hpp"
#include "render_resource.hpp"
#include "forge/concurrency/command_buffer.hpp"
#include "forge/core/isub_system.hpp"
#include "forge/ecs/transform.hpp"
#include "forge/graphics/camera.hpp"
#include "forge/graphics/lights.hpp"
#include "forge/graphics/material.hpp"
#include "forge/graphics/loaders/mesh_load_options.hpp"
#include "forge/memory/mem_pool.hpp"

class GLFWwindow;

#define OGL_MAX_LIGHTS 16

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

	constexpr u8 R_VISIBLE		= 1 << 0;
	constexpr u8 R_CAST_SHADOW	= 1 << 1;
	constexpr u8 R_WIREFRAME	= 1 << 2;
	constexpr u8 R_IS_CULLED	= 1 << 3;

	constexpr u32 R_DEFAULT = R_VISIBLE | R_CAST_SHADOW;

	struct RenderObject
	{
		u32 flags = R_DEFAULT;
		Material material;
		glm::mat4 normal_matrix;
		glm::mat4 model;
		u32 id;

		void compute_model(const glm::mat4 &new_model)
		{
			model = new_model;
			normal_matrix = glm::transpose(glm::inverse(model));
		}
	};

	struct LoadMeshResult
	{
		RenderObject *object = nullptr;
		Transform transform;
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

		LoadMeshResult create_render_object(std::string_view filepath, MeshLoadOptions options = {});
		void destroy_render_object(RenderObject *object);

		// PrimitiveModel* create_primitive(glm::mat4 model = {});

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

		MemPool m_render_data;

		struct TextureData
		{
			OglTexture texture;
			std::string_view path;
			bool is_valid = false;
		};

		struct RenderData
		{
			RenderObject object;
			TextureList<OglTexture> textures;
			OglBuffers buffers;
			u32 index_size {};
			bool in_use = true;
		};

		void handle_framebuffer_resize(int width, int height);
	};

}
