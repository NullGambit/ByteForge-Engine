#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <type_traits>

namespace forge
{
	enum class CameraProjectionMode
	{
		Perspective,
		Orthographic,
	};

	// TODO: the api of the camera class is currently very place holder most of it will likely be replaced in the future
	class Camera
	{
	public:

		glm::vec3 up_dir {0, 1, 0};
		glm::vec3 front_dir {0, 0, -1};
		float yaw;
		float pitch;

		Camera(CameraProjectionMode mode, float fov = 65, float near = 0.1, float far = 1000);

		Camera() = default;

		void set_mode(CameraProjectionMode mode);
		void set_fov(float fov);
		void set_near(float near);
		void set_far(float far);
		void set_bounds(float near, float far);

		inline void set_position(const glm::vec3 position)
		{
			m_position = position;
			// m_view = glm::translate(m_view, position);
		}

		[[nodiscard]]
		inline glm::vec3 get_position() const
		{
			return m_view[3];
		}

		inline void set_direction()
		{
			m_direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
			m_direction.y = glm::sin(glm::radians(pitch));
			m_direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

			front_dir = glm::normalize(m_direction);
		}

		[[nodiscard]]
		inline glm::vec3 get_direction() const
		{
			return m_direction;
		}

		inline void set_zoom(float value)
		{
			m_view = glm::scale(m_view, glm::vec3{value});
		}

		[[nodiscard]]
		inline auto get_front() const
		{
			return glm::normalize(m_direction);
		}

		[[nodiscard]]
		inline auto get_right() const
		{
			return glm::normalize(glm::cross(front_dir, up_dir));
		}

		[[nodiscard]]
		inline auto get_left() const
		{
			return -get_right();
		}

		[[nodiscard]]
		inline auto get_up() const
		{
			return glm::cross(m_direction, get_right());
		}

		[[nodiscard]]
		inline auto get_down() const
		{
			return -get_up();
		}

		inline void look_at(glm::vec3 position, glm::vec3 target)
		{
			m_view = glm::lookAt(position, target, up_dir);
		}

		glm::mat4& get_view()
		{
			return m_view;
		}

		glm::mat4& get_projection()
		{
			return m_projection;
		}

		[[nodiscard]]
		inline glm::mat4 pv() const
		{
			return m_projection * m_view;
		}

		[[nodiscard]]
		inline glm::mat4 pvm(const glm::mat4 &model) const
		{
			return pv() * model;
		}

		void set_projection(CameraProjectionMode mode = CameraProjectionMode::Perspective);

	private:
		glm::mat4 m_view {};
		glm::mat4 m_projection {};
		glm::vec3 m_direction {};
		glm::vec3 m_position;

		float m_fov = 65;
		float m_near = 0.1;
		float m_far = 1000;
		CameraProjectionMode m_projection_mode = CameraProjectionMode::Perspective;
	};
}
