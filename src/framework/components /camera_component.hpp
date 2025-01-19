#pragma once

#include "ecs/ecs.hpp"
#include "graphics/camera.hpp"

#define CAMERA_COMPONENT_EXPORTED_FIELDS forge::FieldSeperator{"Camera"}, &fov, &near, &far, &zoom

namespace forge
{
	class CameraComponent : public IComponent, public Camera
	{
	public:

		EXPORT_FIELDS(CAMERA_COMPONENT_EXPORTED_FIELDS)

		SET_MAX_COMPONENT_MEMORY(sizeof(CameraComponent) * 32)

	private:
		u32 m_on_transform_update_connection {};

	protected:

		void on_destroy() override;

		void on_enabled() override;

		void on_create() override;
	};
}

