#include "ogl_renderer.hpp"

#include "ogl_shader.hpp"
#include "fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "core/engine.hpp"

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


	float vertices[] = {
		// positions        // texture coords
		0.5f,  0.5f, 0.0f,   2.0f, 2.0f,   // top right
		0.5f, -0.5f, 0.0f,   2.0f, 0.0f,   // bottom right
	   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
	   -0.5f,  0.5f, 0.0f,   0.0f, 2.0f    // top left
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	m_wall_texture.load("assets/textures/wall.jpg");
	m_face_texture.load("assets/textures/awesomeface.png", {.flip_on_load = true, .wrap_mode = TextureWrap::Repeat});

	return {};
}

void forge::OglRenderer::update()
{
	m_command_buffer.execute_all();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_wall_texture.bind();
	m_face_texture.bind(1);
	m_tri_shader.use();
	m_tri_shader.set("texture0", 0);
	m_tri_shader.set("texture1", 1);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
