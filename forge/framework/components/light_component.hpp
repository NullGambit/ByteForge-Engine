#pragma once
#include "../../container/array.hpp"
#include "../../ecs/ecs.hpp"
#include "../../graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	class LightComponent : public IComponent
	{
	public:

		Array<ComponentField> export_fields() override;

		Light* get_light();

	protected:
		Light *m_light = nullptr;

		void on_create() override;
		void on_destroy() override;
		void on_enabled() override;
		void on_disabled() override;
	};
}
