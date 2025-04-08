#include "light_component.hpp"

#include "forge/container/array.hpp"
#include "forge/core/engine.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"


void LightComponent::on_create()
{
	auto *renderer = g_engine.get_subsystem<forge::OglRenderer>();

	m_light = renderer->create_light();

	m_light->type = forge::LightType::Point;
	m_light->linear = 0.09f;
	m_light->quadratic = 0.032f;
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
	fields.emplace_back("", forge::ColorField{"color", &m_light->color});

	switch (m_light->type)
	{
		case forge::LightType::Direction:
			fields.emplace_back("direction", &m_light->direction);
			break;
		case forge::LightType::Spot:
			fields.emplace_back("position", &m_light->position);
			fields.emplace_back("direction", &m_light->direction);
			fields.emplace_back("cutoff", &m_light->cutoff);
			fields.emplace_back("outer cutoff", &m_light->outer_cutoff);
			fields.emplace_back("linear", &m_light->linear);
			fields.emplace_back("quadratic", &m_light->quadratic);
			break;
		case forge::LightType::Point:
			fields.emplace_back("position", &m_light->position);
			fields.emplace_back("linear", &m_light->linear);
			fields.emplace_back("quadratic", &m_light->quadratic);
			break;
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
