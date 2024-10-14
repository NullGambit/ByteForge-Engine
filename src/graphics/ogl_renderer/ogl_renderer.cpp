#include "ogl_renderer.hpp"

#include <set>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	C(m_tri_shader.compile({SHADER_PATH"triangles.frag", SHADER_PATH"triangles.vert"}));

#undef C

	constexpr auto MAX_CUBES = 1200;

	f32 batched_cubes[sizeof(CUBE_VERTS) * MAX_CUBES];
	i32 batch_offset = 0;

	for (auto i = 0; i < MAX_CUBES; i++)
	{
		memcpy(batched_cubes + batch_offset, CUBE_VERTS, sizeof(CUBE_VERTS));
		batch_offset += sizeof(CUBE_VERTS);
	}

	uint32_t vbo, ebo;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 5, (void*)(3 * sizeof(f32)));
	glEnableVertexAttribArray(1);

	m_cube_texture.load("assets/textures/smoking_rat.png");

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	srand(time(0));

	std::set<int> pos_set;
	constexpr auto MAX_ATTEMPTS = 100;

	for (int i = 0; i < 1200; i++)
	{
		auto rand_position = util::rand_vec3(-20, 20);
		auto length = glm::length(rand_position) * 2;

		auto attempts = 0;

		while (!pos_set.emplace(length).second && attempts <= MAX_ATTEMPTS)
		{
			rand_position = util::rand_vec3(-20, 20);
			length = glm::length(rand_position) * 2;
			attempts++;
		}

		if (attempts == MAX_ATTEMPTS)
		{
			continue;
		}

		m_cube_positions.emplace_back(rand_position, util::rand_vec3(-1, 1));
	}

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

	m_cube_texture.bind();

	m_tri_shader.use();

	auto runtime = Engine::get_instance().get_engine_runtime();

	glBindVertexArray(m_vao);

	for (const auto &[position, euler_angle] : m_cube_positions)
	{
		glm::mat4 model {1.0};

		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3{0.5});

		model = glm::rotate(model, runtime, euler_angle);

		m_tri_shader.set("pvm", m_pv * model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_statistics.draw_calls++;
	}

	glBindVertexArray(0);
}

void forge::OglRenderer::shutdown()
{
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

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}
