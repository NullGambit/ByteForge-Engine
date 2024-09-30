#include "ogl_renderer.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

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


	uint32_t vbo, ebo;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	m_cube_texture.load("assets/textures/smoking_rat.png");

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	m_camera.set_projection();

	srand(time(0));

	for (int i = 0; i < 25; i++)
	{
		m_cube_positions.emplace_back(util::rand_vec3(-5, 5), util::rand_vec3(0, 360));
	}

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

	for (auto i = 0; const auto &[positon, euler_angle] : m_cube_positions)
	{
		glm::mat4 model {1.0};

		model = glm::translate(model, positon);
		model = glm::scale(model, glm::vec3{0.5});

		model = glm::rotate(model, runtime, euler_angle);

		m_tri_shader.set("pvm", m_pv * model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindVertexArray(0);
}

void forge::OglRenderer::shutdown()
{
}

void forge::OglRenderer::toggle_wireframe()
{
	m_command_buffer.emplace([&draw_wireframe = m_draw_wireframe]
	{
		draw_wireframe = !draw_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, draw_wireframe ? GL_LINE: GL_FILL);
	});
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	glViewport(0, 0, width, height);
}
