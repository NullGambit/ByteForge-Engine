#include "light_component.hpp"

#include "forge/container/array.hpp"
#include "forge/core/engine.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"


void LightComponent::on_create()
{
	auto *renderer = g_engine.get_subsystem<forge::OglRenderer>();

	m_light = renderer->create_light();

	m_light->type = forge::LightType::Point;

	auto &owner = m_owner->get_entity();

	owner.on_transform_update.connect([&light = m_light](forge::Entity &entity)
	{
		const auto &transform = entity.get_top_most_parent_transform();
		light->position = transform.get_local_position();
		light->direction = transform.get_local_euler_rotation();
	});
}

void LightComponent::on_destroy()
{
	auto *renderer = g_engine.get_subsystem<forge::OglRenderer>();

	renderer->destroy_light(m_light);
}

forge::Array<forge::ComponentField> LightComponent::export_fields()
{
	if (m_light == nullptr)
	{
		return {};
	}

	forge::Array<forge::ComponentField> fields;

	fields.reserve(8);

	fields.emplace_back("type", &m_light->type);
	fields.emplace_back("", forge::FieldSeperator{"Properties"});
	fields.emplace_back("Intensity", &m_light->intensity);
	fields.emplace_back("", forge::ColorField{"color", &m_light->color});

	if (m_light->type == forge::LightType::Spot)
	{
		fields.emplace_back("cutoff", &m_light->cutoff);
		fields.emplace_back("outer cutoff", &m_light->outer_cutoff);
	}

	if (m_light->type != forge::LightType::Direction)
	{
		fields.emplace_back("max distance", &m_light->max_distance);
	}

	return fields;
}

forge::Light * LightComponent::get_light()
{
	return m_light;
}

void LightComponent::on_enabled()
{
	if (m_light)
	{
		m_light->enabled = true;
	}
}

void LightComponent::on_disabled()
{
	if (m_light)
	{
		m_light->enabled = false;
	}
}
