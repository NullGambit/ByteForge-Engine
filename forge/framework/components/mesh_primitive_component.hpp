#pragma once

#include "forge/ecs/ecs.hpp"
#include "forge/graphics/material.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

class MeshPrimitiveComponent : public forge::IComponent
{
public:

	void on_editor_controls()
	{
	}

	~MeshPrimitiveComponent() override
	{
		// m_renderer->destroy_primitive(m_data->get_id());
		m_owner->get_entity().on_transform_update.disconnect(m_on_update_connection);
	}

	void set_texture(std::string_view path, u32 type);

	forge::Texture& get_texture(u32 type);

	forge::Material& get_material()
	{
		static forge::Material m;
		return m;
	}

	std::vector<forge::ComponentField> export_fields() override;

private:
	forge::OglRenderer *m_renderer;
	forge::ConnectionID m_on_update_connection;

protected:
	void on_create() override;

	// void on_disabled() override
	// {
	// 	m_data->is_hidden = true;
	// }
	//
	// void on_enabled() override
	// {
	// 	m_data->is_hidden = false;
	// }

};
