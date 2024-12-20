#include "ogl_renderer.hpp"

#include <set>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

#include "gl_buffers.hpp"
#include "ogl_shader.hpp"
#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "core/engine.hpp"
#include "core/logging.hpp"
#include "graphics/mesh_primitives.hpp"
#include "system/fs_monitor.hpp"
#include "util/macros.hpp"
#include "util/random.hpp"

#define CAMERA_POOL_SIZE sizeof(forge::Camera) * 16
#define RENDER_DATA_POOL_SIZE MB(2048)

std::string forge::OglRenderer::init()
{
	auto ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!ok)
	{
		return "could not init glad";
	}

	glClearColor(0, 255, 76, 255);

	auto &engine = Engine::get_instance();

	// TODO: remove connection
	engine.window.on_resize.connect(this, &OglRenderer::handle_framebuffer_resize);

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

	m_cube_buffers = GlBufferBuilder()
		.start()
		.stride(sizeof(f32) * 8)
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

	for (auto i = 0; i < m_render_data_pool.get_length(); i++)
	{
		auto &data = *m_render_data_pool.get_from_index<PrimitiveRenderData>(i);

		if (data.is_valid && !data.primitive.is_hidden)
		{
			const auto &model = data.primitive.m_model;

			const auto pvm = pv * model;

			m_forward_shader.set("pvm", pvm);
			m_forward_shader.set("model", model);
			m_forward_shader.set("view_position", m_active_camera->position);
			m_forward_shader.set("normal_matrix", data.primitive.m_normal_matrix);
			m_forward_shader.set("light_position", m_light_position);
			m_forward_shader.set("light_color", m_light_color);


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
				data.textures[i].texture.bind(i);

				auto &props = properties[i];

				m_forward_shader.set(props[0], i);
				m_forward_shader.set(props[1], texture.enabled);
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
{}

std::vector<forge::DependencyStorage> forge::OglRenderer::get_dependencies()
{
	std::vector<DependencyStorage> dependencies;

#ifdef SHADER_HOT_RELOAD
	dependencies.emplace_back(make_dependency<FsMonitor>());
#endif

	return dependencies;
}

void forge::OglRenderer::receive_cmd_args(ArgParser &parser)
{
	parser.add("shader_path", &m_arg_config.shader_path,
		{.description = "the path where the target shaders are located in", .group = "rendering"});
}

void forge::OglRenderer::start_tick()
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
	auto [ptr, id] = m_render_data_pool.allocate(true);

	auto *data = (PrimitiveRenderData*)ptr;

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
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}

