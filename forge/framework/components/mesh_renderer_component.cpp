#include "mesh_renderer_component.hpp"

#include "../../core/engine.hpp"
#include "../../system/native_dialog.hpp"
#include "../../graphics/ogl_renderer/ogl_renderer.hpp"

forge::Array<forge::ComponentField> MeshRendererComponent::export_fields()
{
	std::vector<forge::ComponentField> out;

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

	auto &material = m_object->material;

	for (auto type = 0; auto &texture : material.textures)
	{
		out.emplace_back(FIELD_ENTRY(forge::FieldSeperator{forge::TextureType::to_string(type)}));
		out.emplace_back<forge::ComponentField>({"Set texture", forge::ButtonField{
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

void MeshRendererComponent::set_mesh(std::string_view path, forge::MeshLoadOptions options)
{
	if (has_mesh())
	{
		reset_mesh();
	}

	auto tree = g_engine.renderer->create_render_object(path, options);

	set_mesh(tree);
}

void MeshRendererComponent::set_mesh(forge::RenderObjectTree &tree)
{
	m_object = tree.object;

	auto &owner = m_owner->get_entity();

	owner.set_transform(tree.transform);

	m_transform_update_con = owner.on_transform_update.connect([&object = m_object](const forge::Entity &entity)
	{
		object->compute_model(entity.get_model());
	});
}

bool MeshRendererComponent::set_texture(std::string_view path, u32 type, forge::TextureOptions options) const
{
	return g_engine.renderer->create_texture(m_object, path, type, options);
}

void MeshRendererComponent::reset_mesh()
{
	g_engine.renderer->destroy_render_object(m_object);

	m_object = nullptr;

	auto &owner = m_owner->get_entity();

	owner.on_transform_update.disconnect(m_transform_update_con);
}

void MeshRendererComponent::on_destroy()
{
	if (m_object)
	{
		reset_mesh();
	}
}

void MeshRendererComponent::on_enabled()
{
	m_object->flags |= forge::R_VISIBLE;
}

void MeshRendererComponent::on_disabled()
{
	m_object->flags = m_object->flags & ~forge::R_VISIBLE;
}
