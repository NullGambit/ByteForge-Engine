#include "mesh_renderer_component.hpp"

#include "../../core/engine.hpp"
#include "../../system/native_dialog.hpp"
#include "../../graphics/ogl_renderer/ogl_renderer.hpp"
#include "forge/graphics/mesh_generator.hpp"

forge::Array<forge::ComponentField> forge::MeshRendererComponent::export_fields()
{
	Array<ComponentField> out;

	out.emplace_back<forge::ComponentField>({"Set mesh", forge::ButtonField{
			[&comp = *this]
			{
				auto file_paths = forge::native_file_dialog({
				.root = "./assets/",
				.allow_multiple = false,
				.allowed_mimes = "model/gltf-binary model/gltf+binary model/obj"});

				if (file_paths.empty())
				{
					return;
				}

				comp.set_mesh(file_paths.front());
			}
		}});

	if (!has_mesh())
	{
		return out;
	}

	auto &material = object->material;

	out.emplace_back<ComponentField>({"color", COLOR_FIELD(&material.color)});

	for (auto type = 0; auto &texture : material.textures)
	{
		out.emplace_back(FIELD_ENTRY(FieldSeperator{TextureType::to_string(type)}));
		out.emplace_back<ComponentField>({"Set texture", ButtonField{
			[&comp = *this, type]
			{
				auto file_paths = forge::native_file_dialog({
				.root = "./assets/",
				.allow_multiple = false,
				.allowed_mimes = "image/png image/jpeg"});

				if (file_paths.empty())
				{
					return;
				}

				comp.set_texture(file_paths.front(), type);
			}
		}});
		out.emplace_back(FIELD_ENTRY(&texture.scale));
		out.emplace_back(FIELD_ENTRY(&texture.enabled));
		out.emplace_back(FIELD_ENTRY(&texture.strength));

		type++;
	}

	return out;
}

void forge::MeshRendererComponent::set_mesh(std::string_view path, MeshLoadOptions options)
{
	if (has_mesh())
	{
		reset_mesh();
	}

	auto tree = g_engine.renderer->create_render_object(path, options);

	set_mesh(tree);
}

// will find the first valid mesh in this tree. for hierarchies set_mesh probably shouldn't be used anyway
forge::RenderObject* find_valid_object(forge::RenderObjectTree &tree)
{
	if (tree.object)
	{
		return tree.object;
	}

	for (auto &child : tree.children)
	{
		auto *object = find_valid_object(child);

		if (object)
		{
			return object;
		}
	}

	return nullptr;
}

void forge::MeshRendererComponent::set_mesh(RenderObjectTree &tree)
{
	object = find_valid_object(tree);

	m_owner->set_transform(tree.transform);

	set_transform_update_con();
}

void forge::MeshRendererComponent::set_mesh(PrimitiveMeshType type)
{
	MeshView mesh;

	switch (type)
	{
		case PrimitiveMeshType::Cube: mesh = generate_cube(); break;
	}

	object = g_engine.renderer->create_render_object(mesh);

	set_transform_update_con();
}

bool forge::MeshRendererComponent::set_texture(std::string_view path, u32 type, TextureOptions options) const
{
	return g_engine.renderer->create_texture(object, path, type, options);
}

void forge::MeshRendererComponent::reset_mesh()
{
	g_engine.renderer->destroy_render_object(object);

	object = nullptr;

	m_owner->on_transform_update.disconnect(m_transform_update_con);
}

void forge::MeshRendererComponent::on_destroy()
{
	if (object)
	{
		reset_mesh();
	}
}

void forge::MeshRendererComponent::on_enabled()
{
	object->flags |= R_VISIBLE;
}

void forge::MeshRendererComponent::on_disabled()
{
	object->flags = object->flags & ~R_VISIBLE;
}

void forge::MeshRendererComponent::set_transform_update_con()
{
	m_transform_update_con = m_owner->on_transform_update.connect([&object = object](const Entity &entity)
	{
		object->compute_model(entity.get_model());
	});
}
