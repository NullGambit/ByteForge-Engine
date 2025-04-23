#include "ogl_renderer.hpp"

#include <set>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../loaders/gltf_loader.hpp"
#include <utility>
#include <GL/glext.h>

#include "ogl_buffers.hpp"
#include "ogl_shader.hpp"
#include "forge/fmt/fmt.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "forge/core/engine.hpp"
#include "forge/core/logging.hpp"
#include "../../math/transform.hpp"
#include "forge/graphics/mesh.hpp"
#include "forge/graphics/mesh_primitives.hpp"
#include "forge/system/fs_monitor.hpp"
#include "forge/util/macros.hpp"
#include "forge/memory/defs.hpp"
#include "forge/system/window_sub_system.hpp"
#include "stb/stb_image.h"

#define CAMERA_POOL_SIZE sizeof(forge::Camera) * 16
#define RENDER_DATA_POOL_SIZE MB(2048)

static constexpr forge::TextureList<std::array<std::string_view, 4>> g_material_prop_str
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	m_active_camera = &m_default_camera;

	m_render_data.init<RenderData>(RENDER_DATA_POOL_SIZE);

	return {};
}

void forge::OglRenderer::update()
{
	m_command_buffer.execute_all();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_forward_shader.use();

	const auto pv = m_active_camera->calculate_pv();

	for (const auto &data : m_render_data.get_iterator<RenderData>())
	{
		if (data.in_use && data.object.flags & R_VISIBLE)
		{
			const auto pvm = pv * data.object.model;

			m_forward_shader.set("pvm", pvm);
			m_forward_shader.set("model", data.object.model);
			m_forward_shader.set("normal_matrix", data.object.normal_matrix);
			m_forward_shader.set("view_position", m_active_camera->position);

			for (auto i = 0; const auto &light : m_lights)
			{
				auto index = fmt::format("[{}]", i++);

				if (light.enabled)
				{

					m_forward_shader.set(fmt::format_view("lights{}.position", index), light.position);
					m_forward_shader.set(fmt::format_view("lights{}.direction", index), light.direction);
					m_forward_shader.set(fmt::format_view("lights{}.color", index), light.color);
					m_forward_shader.set(fmt::format_view("lights{}.intensity", index), light.intensity);
					m_forward_shader.set(fmt::format_view("lights{}.cutoff", index), light.cutoff);
					m_forward_shader.set(fmt::format_view("lights{}.outer_cutoff", index), light.outer_cutoff);
					m_forward_shader.set(fmt::format_view("lights{}.max_distance", index), light.max_distance);
					m_forward_shader.set(fmt::format_view("lights{}.type", index), (int)light.type);
				}

				m_forward_shader.set(fmt::format("lights{}.enabled", index), light.enabled);
			}

			auto &material = data.object.material;

			for (auto i = 0; auto &texture : material.textures)
			{
				auto &texture_data = data.textures[i];

				texture_data.bind(i);

				// Anisotropic filtering
				// TODO: add global settings for this and add a per mesh override
				glTexParameterf(texture_data.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

				auto &props = g_material_prop_str[i];

				auto enable_texture = texture.enabled;

				m_forward_shader.set(props[1], enable_texture);

				m_forward_shader.set(props[0], i);
				m_forward_shader.set(props[2], texture.scale);
				m_forward_shader.set(props[3], texture.strength);

				i++;
			}

			data.buffers.bind();

			glDrawElements(GL_TRIANGLES, data.index_size, GL_UNSIGNED_INT, 0);

			data.buffers.unbind();

			m_statistics.draw_calls++;
		}
	}
}

void forge::OglRenderer::shutdown()
{
	m_render_data.destroy();
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


forge::RenderObjectTree forge::OglRenderer::create_node_buffers(MeshLoaderNode &node)
{
	RenderObjectTree out;

	out.transform = node.transform;

	out.name = node.name;

	if (node.light)
	{
		out.light = node.light;
	}

	for (auto &primitive : node.primitives)
	{
		auto [rd, id] = m_render_data.emplace<RenderData>();

		rd->object.id = id;
		rd->object.flags = R_DEFAULT;
		rd->in_use = true;

		out.object = &rd->object;

		rd->object.compute_model(node.transform.get_global_matrix());
		rd->object.material = primitive.material;
		rd->textures[TextureType::Diffuse].load(primitive.texture);
		rd->index_size = primitive.mesh.indices.size();

		rd->buffers = OglBufferBuilder()
			.start()
			.stride<Vertex>()
			.vbo(primitive.mesh.vertices)
			.ebo(primitive.mesh.indices)
			.attr(3)
			.attr(3)
			.attr(2)
			.finish();
	}

	for (auto &child : node.children)
	{
		out.children.emplace_back(create_node_buffers(child));
	}

	return out;
}

forge::RenderObjectTree forge::OglRenderer::create_render_object(std::string_view filepath, MeshLoadOptions options)
{
	GltfLoader loader;

	auto mesh_opt = loader.load_mesh(filepath, options);

	if (!mesh_opt)
	{
		return {};
	}

	return create_node_buffers(mesh_opt.value());
}

void forge::OglRenderer::destroy_render_object(RenderObject *object)
{
	auto *rd = m_render_data.get<RenderData>(object->id);

	rd->in_use = false;

	m_render_data.free(object->id);
}

void forge::OglRenderer::destroy_texture(std::string_view path)
{

}

bool forge::OglRenderer::create_texture(RenderObject *object, std::string_view path, u32 type, TextureOptions options)
{
	auto *rd = m_render_data.get<RenderData>(object->id);

	auto ok = rd->textures[type].load(path, options);

	if (!ok)
	{
		return false;
	}

	object->material.textures[type].enabled = true;

	return true;
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

forge::Array<forge::Light*> forge::OglRenderer::get_active_lights()
{
	Array<Light*> out;

	out.reserve(OGL_MAX_LIGHTS);

	for (auto &light : m_lights)
	{
		if (!light.is_available)
		{
			out.emplace_back(&light);
		}
	}

	return out;
}

void forge::OglRenderer::handle_framebuffer_resize(int width, int height)
{
	m_command_buffer.emplace([width, height]
	{
		glViewport(0, 0, width, height);
	});
}



