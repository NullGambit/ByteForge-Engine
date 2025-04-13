#include "mesh_primitive_component.hpp"

#include "forge/system/native_dialog.hpp"
#include "forge/core/engine.hpp"

void MeshPrimitiveComponent::set_texture(std::string_view path, u32 type)
{
	if (type >= forge::TextureType::Max)
	{
		return;
	}

	// auto &texture = m_data->material.textures[type];
	//
	// texture.enabled = true;
	//
	// m_renderer->create_texture(m_data->get_id(), path, type);
}

forge::Texture& MeshPrimitiveComponent::get_texture(u32 type)
{
	static forge::Texture t;
	return t;
}

std::vector<forge::ComponentField> MeshPrimitiveComponent::export_fields()
{
	std::vector<forge::ComponentField> out;

	// for (auto type = 0; auto &texture : m_data->material.textures)
	// {
	// 	out.emplace_back(FIELD_ENTRY(forge::FieldSeperator{forge::TextureType::to_string(type)}));
	// 	out.emplace_back<forge::ComponentField>({"Set texture", forge::ButtonField{
	// 		[&comp = *this, type]
	// 		{
	// 			auto file_paths = forge::native_file_dialog({
	// 			.root = "./assets/",
	// 			.allow_multiple = false,
	// 			.allowed_mimes = "image/png image/jpeg"});
	//
	// 			if (file_paths.empty())
	// 			{
	// 				return;
	// 			}
	//
	// 			comp.set_texture(file_paths.front(), type);
	// 		}
	// 	}});
	// 	out.emplace_back(FIELD_ENTRY(&texture.scale));
	// 	out.emplace_back(FIELD_ENTRY(&texture.enabled));
	// 	out.emplace_back(FIELD_ENTRY(&texture.strength));
	//
	// 	type++;
	// }

	return out;
}

void MeshPrimitiveComponent::on_create()
{
	m_renderer = g_engine.get_subsystem<forge::OglRenderer>();

	auto &entity = m_owner->get_entity();
	auto model = entity.get_model();

	// m_data = m_renderer->create_primitive(model);
	//
	// m_on_update_connection = entity.on_transform_update.connect(
	// [&data = m_data](const auto &entity)
	// {
	// 	data->update_model(entity.get_model());
	// });
}