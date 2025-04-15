#pragma once
#include "../../container/array.hpp"
#include "../../ecs/ecs.hpp"
#include "../../graphics/ogl_renderer/ogl_renderer.hpp"

class LightComponent : public forge::IComponent
{
public:

	forge::Array<forge::ComponentField> export_fields() override;

	forge::Light* get_light();

protected:
	forge::Light *m_light = nullptr;

	void on_create() override;
	void on_destroy() override;
	void on_enabled() override;
	void on_disabled() override;
};
