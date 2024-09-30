#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>

namespace forge
{
	enum class CameraProjectionMode
	{
		Perspective,
		Orthographic,
	};

	class Camera
	{
	public:

		glm::vec3 up_dir {0, 1, 0};

		Camera(CameraProjectionMode mode, float fov = 65, float near = 0.1, float far = 1000);

		Camera() = default;

		void set_mode(CameraProjectionMode mode);
		void set_fov(float fov);
		void set_near(float near);
		void set_far(float far);
		void set_bounds(float near, float far);

		inline void set_position(const glm::vec3 position)
		{
			m_view[3] = glm::vec4{position, 1};
			m_view = glm::translate(m_view, position);
		}

		[[nodiscard]]
		inline glm::vec3 get_position() const
		{
			return m_view[3];
		}

		inline void set_direction(const float yaw, const float pitch)
		{
			m_direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
			m_direction.y = glm::sin(glm::radians(pitch));
			m_direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
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
		inline auto get_right() const
		{
			return glm::normalize(glm::cross(up_dir, m_direction));
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

		void set_projection();

	private:
		glm::mat4 m_view {};
		glm::mat4 m_projection {};
		glm::vec3 m_direction {};
		float m_fov = 65;
		float m_near = 0.1;
		float m_far = 1000;
		CameraProjectionMode m_projection_mode = CameraProjectionMode::Perspective;
	};
}
