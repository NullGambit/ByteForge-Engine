#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <type_traits>

namespace forge
{
	enum class CameraProjectionMode : u8
	{
		Perspective,
		Orthographic,
	};

	struct Camera
	{
		glm::vec3 up {0, 1, 0};
		glm::vec3 position {};

		CameraProjectionMode projection_mode = CameraProjectionMode::Perspective;

		float yaw {};
		float pitch {};
		float fov = 65;
		float near = 0.1;
		float far = 1000;
		float zoom = 1.0;

		Camera(CameraProjectionMode mode, float fov = 65, float near = 0.1, float far = 1000) :
			projection_mode(mode),
			fov(fov),
			near(near),
			far(far)
		{}

		Camera() = default;

		[[nodiscard]]
		inline auto get_front() const
		{
			const auto y = glm::radians(yaw);
			const auto p = glm::radians(pitch);

			const glm::vec3 direction
			{
				glm::cos(y) * glm::cos(p),
				glm::sin(p),
				glm::sin(y) * glm::cos(p),
			};

			return glm::normalize(direction);
		}

		[[nodiscard]]
		inline auto get_back() const
		{
			return -get_front();
		}

		[[nodiscard]]
		inline auto get_right() const
		{
			return glm::normalize(glm::cross(get_front(), up));
		}

		[[nodiscard]]
		inline auto get_left() const
		{
			return -get_right();
		}

		[[nodiscard]]
		inline auto get_up() const
		{
			return glm::normalize(glm::cross(get_front(), get_right()));
		}

		[[nodiscard]]
		inline auto get_down() const
		{
			return -get_up();
		}

		[[nodiscard]]
		inline glm::mat4 get_view() const
		{
			auto view = glm::lookAt(position, position + get_front(), up);

			view = glm::scale(view, glm::vec3{zoom});

			return view;
		}

		[[nodiscard]]
		glm::mat4 get_projection() const;

		[[nodiscard]]
		inline glm::mat4 calculate_pv() const
		{
			return get_projection() * get_view();
		}

		[[nodiscard]]
		inline glm::mat4 calculate_pvm(const glm::mat4 &model) const
		{
			return calculate_pv() * model;
		}
	};
}
