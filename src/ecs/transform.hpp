#pragma once

#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace forge
{
	class Entity;

	class Transform final
	{
	public:
		inline void set_local_position(glm::vec3 position)
		{
			m_position = position;
			m_is_dirty = true;
		}

		inline glm::vec3 get_local_position() const
		{
			return m_position;
		}

		inline void set_local_rotation(glm::vec3 euler_rotation)
		{
			m_rotation = euler_rotation;
			m_is_dirty = true;
		}

		inline void set_local_rotation(glm::quat rotation)
		{
			m_rotation = rotation;
			m_is_dirty = true;
		}

		inline glm::quat get_local_rotation() const
		{
			return m_rotation;
		}

		inline glm::vec3 get_local_euler_rotation() const
		{
			return glm::eulerAngles(m_rotation);
		}

		inline void set_local_scale(glm::vec3 scale)
		{
			m_scale = scale;
			m_is_dirty = true;
		}

		inline glm::vec3 get_local_scale() const
		{
			return m_scale;
		}

		inline void set_position(glm::vec3 parent, glm::vec3 position)
		{
			m_position = parent * position;
			m_is_dirty = true;
		}

		inline glm::vec3 get_position(glm::vec3 parent) const
		{
			return parent * m_position;
		}

		inline void set_rotation(glm::vec3 parent, glm::vec3 euler_rotation)
		{
			m_rotation = parent * glm::radians(euler_rotation);
			m_is_dirty = true;
		}

		inline void set_rotation(glm::quat parent, glm::quat rotation)
		{
			m_rotation = parent * rotation;
			m_is_dirty = true;
		}

		inline glm::quat get_rotation(glm::quat parent) const
		{
			return parent * m_rotation;
		}

		inline glm::vec3 get_euler_rotation(glm::vec3 parent) const
		{
			return parent * glm::eulerAngles(m_rotation);
		}

		inline void set_scale(glm::vec3 parent, glm::vec3 scale)
		{
			m_scale = parent * scale;
			m_is_dirty = true;
		}

		inline glm::vec3 get_scale(glm::vec3 parent) const
		{
			return parent * m_scale;
		}

		inline glm::mat4 get_model() const
		{
			return m_model;
		}

	private:
		friend Entity;

		bool m_is_dirty = false;

		glm::vec3 m_position {0.0};
		glm::vec3 m_scale {1.0};
		glm::quat m_rotation {1.0f, 0.0f, 0.0f, 0.0f};
		glm::mat4 m_model {1.0};

		[[nodiscard]]
		inline glm::mat4 compute_local_transform()
		{
			auto rotation_matrix	= glm::toMat4(m_rotation);
			auto translation_matrix = glm::translate(glm::mat4(1.0f), m_position);
			auto scale_matrix		= glm::scale(glm::mat4(1.0f), m_scale);

			m_is_dirty = false;

			return translation_matrix * rotation_matrix * scale_matrix;
		}
	};
}
