#include "ogl_renderer.hpp"

#include <set>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

#include "ogl_buffers.hpp"
#include "ogl_shader.hpp"
#include "forge/fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "forge/core/engine.hpp"
#include "forge/core/logging.hpp"
#include "forge/graphics/mesh_primitives.hpp"
#include "forge/system/fs_monitor.hpp"
#include "forge/util/macros.hpp"
#include "forge/memory/defs.hpp"
#include "forge/system/window_sub_system.hpp"

#define CAMERA_POOL_SIZE sizeof(forge::Camera) * 16
#define RENDER_DATA_POOL_SIZE MB(2048)

std::string forge::OglRenderer::init(const EngineInitOptions &options)
{
	auto ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!ok)
	{
		return "could not init glad";
	}

	glClearColor(0, 255, 76, 255);

	// TODO: remove connection
	g_main_window->on_resize.connect(this, &OglRenderer::handle_framebuffer_resize);

	// load builtin shaders

#define C(expr) \
	if (!(expr)) return "could not load or compile builtin shader" \

	auto shader_path = std::string{m_arg_config.shader_path};

	if (!std::filesystem::exists(shader_path))
	{
		return "engine shader path does not exist";
	}

	C(m_forward_shader.compile({shader_path + "forward_lighting.frag", shader_path + "forward_lighting.vert"}));

#undef C

	m_cube_buffers = OglBufferBuilder()
		.start()
		.stride<glm::vec3, glm::vec3, glm::vec2>()
		.vbo(std::span{CUBE_VERTS, sizeof(CUBE_VERTS)})
		.attr(3)
		.attr(3)
		.attr(2)
		.finish();

	m_texture_resource.on_resource_init = [](std::string_view path, OglTexture *texture)
	{
		texture->load(path);
	};

	m_texture_resource.on_resource_deinit = [](OglTexture *texture)
	{
		texture->destroy();
	};

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	m_active_camera = &m_default_camera;

	m_render_data_pool.init<PrimitiveRenderData>(RENDER_DATA_POOL_SIZE);

	return {};
}

void forge::OglRenderer::update()
{
	m_command_buffer.execute_all();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_forward_shader.use();

	glBindVertexArray(m_cube_buffers.vao);

	const auto pv = m_active_camera->calculate_pv();

	for (auto &data : m_render_data_pool.get_iterator<PrimitiveRenderData>())
	{
		if (data.is_valid && !data.primitive.is_hidden)
		{
			const auto &model = data.primitive.m_model;

			const auto pvm = pv * model;

			m_forward_shader.set("pvm", pvm);
			m_forward_shader.set("model", model);
			m_forward_shader.set("normal_matrix", data.primitive.m_normal_matrix);
			m_forward_shader.set("view_position", m_active_camera->position);

			for (auto i = 0; const auto &light : m_lights)
			{
				if (light.enabled)
				{
					auto index = fmt::format("[{}]", i++);

					m_forward_shader.set(fmt::format("lights{}.position", index), light.position);
					m_forward_shader.set(fmt::format("lights{}.direction", index), light.direction);
					m_forward_shader.set(fmt::format("lights{}.color", index), light.color);
					m_forward_shader.set(fmt::format("lights{}.cutoff", index), light.cutoff);
					m_forward_shader.set(fmt::format("lights{}.outer_cutoff", index), light.outer_cutoff);
					m_forward_shader.set(fmt::format("lights{}.linear", index), light.linear);
					m_forward_shader.set(fmt::format("lights{}.quadratic", index), light.quadratic);
					m_forward_shader.set(fmt::format("lights{}.enabled", index), light.enabled);
					m_forward_shader.set(fmt::format("lights{}.type", index), (int)light.type);
				}
			}

			static constexpr TextureList<std::array<std::string_view, 4>> properties
			{
				std::array<std::string_view, 4>
				{
					"material.diffuse.texture",
					"material.diffuse.enabled",
					"material.diffuse.scale",
					"material.diffuse.strength",
				},
				{
					"material.specular.texture",
					"material.specular.enabled",
					"material.specular.scale",
					"material.specular.strength",
				},
				{
					"material.emissive.texture",
					"material.emissive.enabled",
					"material.emissive.scale",
					"material.emissive.strength",
				}
			};

			auto &material = data.primitive.material;

			for (auto i = 0; auto &texture : material.textures)
			{
				auto &texture_data = data.textures[i];

				texture_data.texture.bind(i);

				auto &props = properties[i];

				auto enable_texture = texture_data.is_valid && texture.enabled;

				m_forward_shader.set(props[1], enable_texture);

				m_forward_shader.set(props[0], i);
				m_forward_shader.set(props[2], texture.scale);
				m_forward_shader.set(props[3], texture.strength);

				i++;
			}

			glDrawArrays(GL_TRIANGLES, 0, 36);

			m_statistics.draw_calls++;
		}
	}

	glBindVertexArray(0);
}

void forge::OglRenderer::shutdown()
{
	m_render_data_pool.destroy();
}

std::vector<forge::DependencyStorage> forge::OglRenderer::get_dependencies()
{
	std::vector<DependencyStorage> dependencies;

	dependencies.emplace_back(make_dependency<WindowSubSystem>());

#ifdef FORGE_SHADER_HOT_RELOAD
	dependencies.emplace_back(make_dependency<FsMonitor>());
#endif

	return dependencies;
}

void forge::OglRenderer::receive_cmd_args(ArgParser &parser)
{
	parser.add("shader_path", &m_arg_config.shader_path,
		{.description = "the path where the target shaders are located in", .group = "rendering"});
}

void forge::OglRenderer::pre_update()
{
	m_statistics = {};
}

void forge::OglRenderer::set_wireframe(bool enable)
{
	m_command_buffer.emplace([enable]
	{
		glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
	});
}

void forge::OglRenderer::set_clear_color(glm::vec3 color)
{
	m_command_buffer.emplace([color]
	{
		glClearColor(EXPAND_VEC3(color), 255);
	});
}

glm::vec3 forge::OglRenderer::get_clear_color()
{
	glm::vec3 out;
	glGetFloatv(GL_COLOR_CLEAR_VALUE, glm::value_ptr(out));
	return out;
}

void forge::OglRenderer::set_active_camera(Camera *camera)
{
	if (camera == nullptr)
	{
		m_active_camera = &m_default_camera;
	}
	else
	{
		m_active_camera = camera;
	}
}

forge::Camera* forge::OglRenderer::get_active_camera()
{
	return m_active_camera;
}

forge::PrimitiveModel* forge::OglRenderer::create_primitive(glm::mat4 model)
{
	auto [data, id] = m_render_data_pool.emplace<PrimitiveRenderData>();

	data->primitive = PrimitiveModel {model};
	data->primitive.m_id = id;
	data->is_valid = true;

	return &data->primitive;
}

void forge::OglRenderer::destroy_primitive(u32 id)
{
	auto *data = m_render_data_pool.get<PrimitiveRenderData>(id);
	data->is_valid = false;

	for (auto &texture_data : data->textures)
	{
		m_texture_resource.remove(texture_data.path);
	}
}

void forge::OglRenderer::destroy_texture(std::string_view path)
{
	m_texture_resource.remove(path);
}

void forge::OglRenderer::create_texture(u32 id, std::string_view path, u32 type)
{
	auto *data = m_render_data_pool.get<PrimitiveRenderData>(id);
	auto &texture_data = data->textures[type];

	if (!texture_data.path.empty())
	{
		destroy_texture(texture_data.path);
	}

	texture_data.texture = m_texture_resource.add(path);
	texture_data.path = path;
	texture_data.is_valid = true;
}

forge::Light * forge::OglRenderer::create_light()
{
	Light *light = nullptr;

	for (auto &l : m_lights)
	{
		if (l.is_available)
		{
			light = &l;
		}
	}

	if (light == nullptr)
	{
		return nullptr;
	}

	light->is_available = false;
	light->enabled      = true;

	return light;
}

void forge::OglRenderer::destroy_light(Light *light)
{
	if (light == nullptr)
	{
		return;
	}

	light->is_available = true;
	light->enabled = false;
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}



