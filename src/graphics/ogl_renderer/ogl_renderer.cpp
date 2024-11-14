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
#include "util/macros.hpp"
#include "util/random.hpp"

#define SHADER_PATH "./assets/shaders/"

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

	if (!std::filesystem::exists(SHADER_PATH))
	{
		return "engine shader path does not exist";
	}

	C(m_forward_shader.compile({SHADER_PATH"forward_lighting.frag", SHADER_PATH"forward_lighting.vert"}));

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

	m_camera_pool.init<Camera>(CAMERA_POOL_SIZE);

	// default camera
	auto [ptr, _] = m_camera_pool.emplace<Camera>();

	m_active_camera = ptr;

	m_render_data_pool.init<PrimitiveRenderData>(RENDER_DATA_POOL_SIZE);

	return {};
}

void forge::OglRenderer::update()
{
	m_command_buffer.execute_all();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_forward_shader.use();

	glBindVertexArray(m_cube_buffers.vao);

	for (auto i = 0; i < m_render_data_pool.get_length(); i++)
	{
		auto &data = *m_render_data_pool.get_from_index<PrimitiveRenderData>(i);

		if (data.is_valid && !data.primitive.is_hidden)
		{
			const auto &model = data.primitive.m_model;

			auto pvm = m_active_camera->get_projection() * m_active_camera->get_view() * model;

			m_forward_shader.set("pvm", pvm);
			m_forward_shader.set("model", model);
			m_forward_shader.set("view_position", m_active_camera->get_position());
			m_forward_shader.set("normal_matrix", data.primitive.m_normal_matrix);
			m_forward_shader.set("light_position", m_light_position);
			m_forward_shader.set("light_color", m_light_color);

			auto &material = data.primitive.material;

			m_forward_shader.set("material.shininess", material.shininess);
			m_forward_shader.set("material.ambient", material.ambient);

			for (auto i = 0; auto &texture : material.textures)
			{
				static char name_buffer[48] = "material.";
				constexpr auto start_offset = sizeof("material.")-1;

				data.textures[i].bind(i);

				auto field_name = TextureType::to_string(i);

				strcpy(name_buffer + start_offset, field_name.data());

				auto last_offset = start_offset + field_name.size();

				name_buffer[last_offset] = '.';
				name_buffer[++last_offset] = '\0';

				strcpy(name_buffer + last_offset, "texture");

				m_forward_shader.set(name_buffer, i);

				strcpy(name_buffer + last_offset, "enabled");

				m_forward_shader.set(name_buffer, texture.enabled);

				strcpy(name_buffer + last_offset, "scale");

				m_forward_shader.set(name_buffer, texture.scale);

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

std::pair<forge::Camera*, u32> forge::OglRenderer::create_camera()
{
	if (m_camera_pool.get_offset() >= CAMERA_POOL_SIZE)
	{
		return {nullptr, UINT32_MAX};
	}

	return m_render_data_pool.emplace<Camera>();
}

void forge::OglRenderer::destroy_camera(u32 id)
{
	m_camera_pool.free(id, false);
}

void forge::OglRenderer::set_active_camera(Camera* camera)
{
	m_active_camera = camera;
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

	for (auto &texture : data->primitive.material.textures)
	{
		if (texture.enabled)
		{
			m_texture_resource.remove(texture.path);
		}
	}
}

void forge::OglRenderer::destroy_texture(std::string_view path)
{
	m_texture_resource.remove(path);
}

void forge::OglRenderer::create_texture(u32 id, std::string_view path, u32 type)
{
	auto *data = m_render_data_pool.get<PrimitiveRenderData>(id);

	data->textures[type] = m_texture_resource.add(path);
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}

