#include "light_component.hpp"

#include "../../container/array.hpp"
#include "../../core/engine.hpp"
#include "../../graphics/ogl_renderer/ogl_renderer.hpp"


void forge::LightComponent::on_create()
{
	auto *renderer = g_engine.get_subsystem<OglRenderer>();

	m_light = renderer->create_light();

	m_owner->on_transform_update.connect([&light = m_light](Entity &entity)
	{
		light->position = entity.get_position();
		light->direction = entity.get_euler_rotation();
	});
}

void forge::LightComponent::on_destroy()
{
	auto *renderer = g_engine.get_subsystem<OglRenderer>();

	renderer->destroy_light(m_light);
}

forge::Array<forge::ComponentField> forge::LightComponent::export_fields()
{
	if (m_light == nullptr)
	{
		return {};
	}

	Array<ComponentField> fields;

	fields.reserve(8);

	fields.emplace_back("type", &m_light->type);
	fields.emplace_back("", FieldSeperator{"Properties"});
	fields.emplace_back("Intensity", &m_light->intensity);
	fields.emplace_back("", ColorField{"color", &m_light->color});

	if (m_light->type == LightType::Spot)
	{
		fields.emplace_back("cutoff", &m_light->cutoff);
		fields.emplace_back("outer cutoff", &m_light->outer_cutoff);
	}

	if (m_light->type != LightType::Direction)
	{
		fields.emplace_back("max distance", &m_light->max_distance);
	}

	return fields;
}

forge::Light * forge::LightComponent::get_light()
{
	return m_light;
}

void forge::LightComponent::on_enabled()
{
	if (m_light)
	{
		m_light->enabled = true;
	}
}

void forge::LightComponent::on_disabled()
{
	if (m_light)
	{
		m_light->enabled = false;
	}
}
