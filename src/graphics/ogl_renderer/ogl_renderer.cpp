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

	srand(time(0));

	auto &window = Engine::get_instance().window;

	auto win_size = window.get_size();

	// set default projection view matrix
	m_pv = glm::perspective(glm::radians(65.0f), (float)win_size.x / win_size.y, 0.1f, 1000.0f);

	return {};
}

void forge::OglRenderer::update()
{
	m_command_buffer.execute_all();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_forward_shader.use();

	glBindVertexArray(m_cube_buffers.vao);

	for (auto &data : m_cube_positions)
	{
		if (data.is_valid && !data.is_hidden)
		{
			data.diffuse_texture.bind();

			m_forward_shader.set("pvm", m_pv * data.primitive.model);
			m_forward_shader.set("model", data.primitive.model);
			m_forward_shader.set("material_color", data.primitive.material.color);
			m_forward_shader.set("light_position", m_light_position);
			m_forward_shader.set("light_color", m_light_color);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			m_statistics.draw_calls++;


			data.diffuse_texture.unbind();
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

u32 forge::OglRenderer::create_primitive(PrimitiveModel primitive)
{
	auto id = get_free_rdi();

	auto &data = m_cube_positions[id];

	data.primitive = primitive;
	data.is_valid = true;

	if (!primitive.material.diffuse.path.empty())
	{
		data.diffuse_texture = m_texture_resource.add(primitive.material.diffuse.path);
	}

	return id;
}

void forge::OglRenderer::update_primitive(u32 id, PrimitiveModel primitive)
{
	m_cube_positions[id].primitive = std::move(primitive);

	// update_primitive_material(id, primitive.material);
}

void forge::OglRenderer::update_primitive_material(u32 id, Material &material)
{
	auto &data = m_cube_positions[id];

	auto &diffuse_path = data.primitive.material.diffuse.path;

	m_texture_resource.remove(diffuse_path);

	data.diffuse_texture = m_texture_resource.add(material.diffuse.path);

	data.primitive.material = material;
}

void forge::OglRenderer::destroy_primitive(u32 id)
{
	auto &data = m_cube_positions[id];
	data.is_valid = false;
	m_texture_resource.remove(data.primitive.material.diffuse.path);
}

void forge::OglRenderer::primitive_set_hidden(u32 id, bool value)
{
	m_cube_positions[id].is_hidden = value;
}

void forge::OglRenderer::destroy_texture(std::string_view path)
{
	m_texture_resource.remove(path);
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}

u32 forge::OglRenderer::get_free_rdi()
{
	for (u32 i = 0; i < m_cube_positions.size(); i++)
	{
		if (!m_cube_positions[i].is_valid)
		{
			return i;
		}
	}

	m_cube_positions.emplace_back();

	return m_cube_positions.size() - 1;
}
