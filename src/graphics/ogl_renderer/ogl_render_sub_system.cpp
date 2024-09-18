#include "ogl_render_sub_system.hpp"

#include "ogl_shader.hpp"
#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "core/engine.hpp"

#define SHADER_PATH "./assets/shaders/"

std::string forge::OglRenderSubSystem::init()
{
	auto ok = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!ok)
	{
		return "could not init glad";
	}

	glClearColor(0, 255, 76, 255);

	auto &engine = Engine::get_instance();

	// TODO: remove connection
	engine.window.on_resize.connect(this, &OglRenderSubSystem::handle_framebuffer_resize);

	// load builtin shaders

#define C(expr) \
	if (!(expr)) return "could not load or compile builtin shader" \

	C(m_tri_shader.compile({SHADER_PATH"triangles.frag", SHADER_PATH"triangles.vert"}));

#undef C

	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
	   -0.5f, -0.5f, 0.0f,  // bottom left
	   -0.5f,  0.5f, 0.0f   // top left
   };

	uint32_t indices[] =
	{  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	uint32_t vbo, ebo;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	return {};
}

void forge::OglRenderSubSystem::update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_tri_shader.use();
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void forge::OglRenderSubSystem::shutdown()
{
}

void forge::OglRenderSubSystem::toggle_wireframe()
{
	m_draw_wireframe = !m_draw_wireframe;
	glPolygonMode(GL_FRONT_AND_BACK, m_draw_wireframe ? GL_LINE: GL_FILL);
}

void forge::OglRenderSubSystem::handle_framebuffer_resize(int width, int height)
{
	glViewport(0, 0, width, height);
}
