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
			auto radians = glm::eulerAngles(m_rotation);

			return {glm::degrees(radians.x), glm::degrees(radians.y), glm::degrees(radians.z)};
		}

		inline void set_scale(glm::vec3 scale)
		{
			m_scale = scale;
			m_is_dirty = true;
		}

		inline glm::vec3 get_scale() const
		{
			return m_scale;
		}

		inline void set_position(glm::vec3 position)
		{
			if (m_parent)
			{
				const auto inverse_parent_matrix = glm::inverse(m_parent->m_global_matrix);
				const auto local_pos			 = inverse_parent_matrix * glm::vec4(position, 1.0f);

				set_local_position(glm::vec3(local_pos));
			}
			else
			{
				set_local_position(position);
			}

			m_is_dirty = true;
		}

		inline glm::vec3 get_position() const
		{
			return glm::vec3{m_global_matrix[3]};
		}

		inline void set_rotation(glm::vec3 euler_rotation)
		{
			set_rotation(glm::quat{glm::radians(euler_rotation)});
			m_is_dirty = true;
		}

		inline void set_rotation(glm::quat rotation)
		{
			if (m_parent)
			{
				glm::quat parentGlobalRot = m_parent->get_global_matrix();
				auto localRot = glm::inverse(parentGlobalRot) * rotation;

				set_local_rotation(localRot);
			}
			else
			{
				set_local_rotation(rotation);
			}

			m_is_dirty = true;
		}

		inline void set_model(glm::mat4 model)
		{
			m_global_matrix = model;
			m_is_dirty = true;
		}

		inline glm::quat get_rotation() const
		{
			if (m_parent)
			{
				return m_parent->get_rotation() * m_rotation;
			}

			return m_rotation;
		}

		inline glm::vec3 get_euler_rotation() const
		{
			return glm::eulerAngles(get_rotation());
		}

		inline const glm::mat4& get_global_matrix() const
		{
			return m_global_matrix;
		}

		inline glm::mat4 get_global_matrix()
		{
			return m_global_matrix;
		}

		inline bool is_dirty() const
		{
			return m_is_dirty;
		}

		inline glm::mat4 compute_local_transform()
		{
			glm::mat4 unit {1.0f};

			auto T = glm::translate(unit, m_position);
			auto R = glm::toMat4(m_rotation);
			auto S = glm::scale(unit, m_scale);

			m_is_dirty = false;

			return T * R * S;
		}

	private:
		friend Entity;
		bool m_is_dirty = false;

		Transform *m_parent = nullptr;

		glm::vec3 m_position {0.0};
		glm::vec3 m_scale {1.0};
		glm::quat m_rotation {1.0f, 0.0f, 0.0f, 0.0f};
		glm::mat4 m_global_matrix {1.0};
	};
}
