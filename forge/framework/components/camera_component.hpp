#pragma once

#include "forge/ecs/ecs.hpp"
#include "forge/graphics/camera.hpp"

#define CAMERA_COMPONENT_EXPORTED_FIELDS forge::FieldSeperator{"Camera"}, &fov, &near, &far, &zoom

namespace forge
{
	class CameraComponent : public IComponent, public Camera
	{
	public:

		REGISTER_UPDATE_FUNC

		EXPORT_FIELDS(CAMERA_COMPONENT_EXPORTED_FIELDS)

		// cant imagine ill ever use more than 8 cameras. even 8 is a bit of a stretch
		SET_MAX_COMPONENT_MEMORY(sizeof(CameraComponent) * 8)

	private:
		u32 m_on_transform_update_connection {};
		glm::quat m_previous_rotation = glm::identity<glm::quat>();

	protected:

		void on_destroy() override;

		void on_enabled() override;

		void on_create() override;
	};
}

