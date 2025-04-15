#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
#include "gltf_loader.hpp"

#include <optional>
#include <glm/gtc/type_ptr.hpp>

#include "mesh_loader.hpp"
#include "forge/ecs/transform.hpp"
#include "forge/graphics/lights.hpp"
#include "forge/graphics/mesh.hpp"

namespace forge
{
	MeshLoaderNode load_node(cgltf_node *node, MeshLoadOptions options)
	{
		MeshLoaderNode out;

		out.name = node->name;

		auto *mesh = node->mesh;

		out.children.reserve(node->children_count);

		for (cgltf_size i = 0; i < node->children_count; ++i)
		{
			auto *child = node->children[i];

			out.children.emplace_back(load_node(child, options));
		}

		if (node->has_scale)
		{
			out.transform.set_local_scale(glm::make_vec3(node->scale));
		}
		if (node->has_translation)
		{
			out.transform.set_local_position(glm::make_vec3(node->translation));
		}
		if (node->has_rotation)
		{
			out.transform.set_local_rotation(glm::make_quat(node->rotation));
		}

		auto model = node->has_matrix ? glm::make_mat4(node->matrix) : glm::mat4{1.0};

		if (node->parent && node->parent->has_matrix)
		{
			model = glm::make_mat4(node->parent->matrix) * model;
		}

		out.transform.set_model(model);
		out.transform.compute_local_transform();

		if (node->light != nullptr)
		{
			Light light;

			light.position = out.transform.get_local_position();
			light.color = glm::make_vec3(node->light->color);
			light.type = LightType::Point;
			light.enabled = true;
			light.is_available = false;

			out.light = light;
		}

		if (mesh == nullptr)
		{
			return out;
		}

		out.primitives.resize(mesh->primitives_count);

		for (cgltf_size p = 0; p < mesh->primitives_count; ++p)
		{
			auto *prim = &mesh->primitives[p];

			cgltf_accessor *pos_accessor = nullptr;
			cgltf_accessor *tex_accessor = nullptr;
			cgltf_accessor *norm_accessor = nullptr;

			for (cgltf_size a = 0; a < prim->attributes_count; ++a)
			{
				auto *attr = &prim->attributes[a];
				auto *accessor = attr->data;

				if (strcmp(attr->name, "POSITION") == 0)
				{
					pos_accessor = accessor;
				}
				else if (strcmp(attr->name, "NORMAL") == 0)
				{
					norm_accessor = accessor;
				}
				else if (strcmp(attr->name, "TEXCOORD_0") == 0)
				{
					tex_accessor = accessor;
				}
			}

			out.primitives[p].mesh.vertices.reserve(pos_accessor->count);
			out.primitives[p].mesh.indices.reserve(prim->indices->count);

			for (cgltf_size i = 0; i < pos_accessor->count; i++)
			{
				Vertex vertex {};

				cgltf_accessor_read_float(pos_accessor, i, glm::value_ptr(vertex.position), 3);
				cgltf_accessor_read_float(tex_accessor, i, glm::value_ptr(vertex.texture), 2);
				cgltf_accessor_read_float(norm_accessor, i, glm::value_ptr(vertex.normals), 3);

				vertex.position *= options.uniform_scale;

				out.primitives[p].mesh.vertices.emplace_back(vertex);
			}

			for (cgltf_size i = 0; i < prim->indices->count; i++)
			{
				auto index = cgltf_accessor_read_index(prim->indices, i);

				out.primitives[p].mesh.indices.emplace_back(index);
			}

			auto *texture = prim->material ? prim->material->pbr_metallic_roughness.base_color_texture.texture : nullptr;

			if (prim->material && prim->material->has_pbr_metallic_roughness && texture)
			{
				auto &tex_view = prim->material->pbr_metallic_roughness.base_color_texture;
				auto *image = texture->image;

				std::string_view buffer;

				if (image->uri)
				{
					buffer = image->uri;
				}
				else
				{
					auto *view = image->buffer_view;

					buffer = std::string_view{(char*)view->buffer->data + view->offset, view->size};
				}

				out.primitives[p].texture.load(buffer,
				{
					.from_memory = image->uri == nullptr
				});

				auto tex_scale = tex_view.transform.scale;

				out.primitives[p].material.textures[TextureType::Diffuse] =
				{
					.scale = tex_view.has_transform ? std::max(tex_scale[0], tex_scale[1]) : 1,
					.enabled = true,
				};
			}
		}

		return out;
	}

	std::optional<MeshLoaderNode> GltfLoader::load_mesh(std::string_view filepath, MeshLoadOptions options)
	{
		MeshLoaderNode out {};
		cgltf_options gltf_options {};
		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&gltf_options, filepath.data(), &data);

		if (result != cgltf_result_success)
		{
			return std::nullopt;
		}

		result = cgltf_load_buffers(&gltf_options, data, "model.gltf");

		if (result != cgltf_result_success)
		{
			return std::nullopt;
		}

		auto *root = data->scene->nodes[0];

		out = load_node(root, options);

		cgltf_free(data);

		return out;
	}
}
