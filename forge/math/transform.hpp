#pragma once

#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace forge
{
	struct Basis
	{
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 forward;
	};

	class Transform final
	{
	public:
		Transform *parent = nullptr;
		bool is_dirty = false;

		inline void set_local_position(glm::vec3 position)
		{
			m_position = position;
			is_dirty = true;
		}

		inline glm::vec3 get_local_position() const
		{
			return m_position;
		}

		inline void set_local_rotation(glm::vec3 euler_rotation)
		{
			m_rotation = glm::quat{glm::eulerAngleYXZ(euler_rotation.y, euler_rotation.x, euler_rotation.z)};

			is_dirty = true;
		}

		void rotate(float yaw_deg, float pitch_deg, float roll_deg)
		{
			auto yaw	= glm::angleAxis(glm::radians(yaw_deg), glm::vec3(0, 1, 0));
			auto pitch	= glm::angleAxis(glm::radians(pitch_deg), glm::vec3(1, 0, 0));
			auto roll	= glm::angleAxis(glm::radians(roll_deg), glm::vec3(0, 0, 1));

			m_rotation = yaw * pitch * roll;

			is_dirty = true;
		}

		inline void set_local_rotation(glm::quat rotation)
		{
			m_rotation = rotation;
			is_dirty = true;
		}

		inline glm::quat get_local_rotation() const
		{
			return m_rotation;
		}

		inline glm::vec3 get_local_euler_rotation() const
		{
			return glm::eulerAngles(m_rotation);
		}

		inline void set_scale(glm::vec3 scale)
		{
			m_scale = scale;
			is_dirty = true;
		}

		inline glm::vec3 get_scale() const
		{
			return m_scale;
		}

		inline void set_position(glm::vec3 position)
		{
			if (parent)
			{
				const auto inverse_parent_matrix = glm::inverse(parent->m_global_matrix);
				const auto local_pos			 = inverse_parent_matrix * glm::vec4(position, 1.0f);

				set_local_position(glm::vec3(local_pos));
			}
			else
			{
				set_local_position(position);
			}

			is_dirty = true;
		}

		inline glm::vec3 get_position() const
		{
			return glm::vec3{m_global_matrix[3]};
		}

		inline void set_rotation(glm::vec3 euler_rotation)
		{
			set_rotation(glm::quat{glm::radians(euler_rotation)});
			is_dirty = true;
		}

		inline void set_rotation(glm::quat rotation)
		{
			if (parent)
			{
				glm::quat parentGlobalRot = parent->get_global_matrix();
				auto localRot = glm::inverse(parentGlobalRot) * rotation;

				set_local_rotation(localRot);
			}
			else
			{
				set_local_rotation(rotation);
			}

			is_dirty = true;
		}

		inline void set_model(glm::mat4 model)
		{
			m_global_matrix = model;
			is_dirty = true;
		}

		inline glm::quat get_rotation() const
		{
			if (parent)
			{
				return parent->get_rotation() * m_rotation;
			}

			return m_rotation;
		}

		inline glm::vec3 get_euler_rotation() const
		{
			return glm::degrees(glm::eulerAngles(get_rotation()));
		}

		inline const glm::mat4& get_global_matrix() const
		{
			return m_global_matrix;
		}

		inline glm::mat4 get_global_matrix()
		{
			return m_global_matrix;
		}

		inline glm::mat4 compute_local_transform()
		{
			glm::mat4 identity {1.0f};

			auto T = glm::translate(identity, m_position);
			auto R = glm::toMat4(m_rotation);
			auto S = glm::scale(identity, m_scale);

			return T * R * S;
		}

		[[nodiscard]]
		inline auto get_forward() const
		{
			const auto y = m_rotation.y;
			const auto p = m_rotation.x;

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
			return -get_forward();
		}

		[[nodiscard]]
		inline auto get_right(glm::vec3 up = {0, 1, 0}) const
		{
			return glm::normalize(glm::cross(get_forward(), up));
		}

		[[nodiscard]]
		inline auto get_left() const
		{
			return -get_right();
		}

		[[nodiscard]]
		inline auto get_up() const
		{
			return glm::normalize(glm::cross(get_forward(), get_right()));
		}

		[[nodiscard]]
		inline glm::mat3 get_basis_mat(bool include_scale = false) const
		{
			auto rotation_matrix = glm::toMat3(m_rotation);

			if (include_scale) [[unlikely]]
			{
				return glm::mat3
				{
				    rotation_matrix[0] * m_scale.x,
				    rotation_matrix[1] * m_scale.y,
				    rotation_matrix[2] * m_scale.z
				};
			}

			return rotation_matrix;
		}

		[[nodiscard]]
		inline Basis get_basis(bool include_scale = false) const
		{
			auto basis = get_basis_mat(include_scale);

			return
			{
				.right = basis[0],
				.up = basis[1],
				.forward = basis[2],
			};
		}

		void compute_global()
		{
			if (parent != nullptr)
			{
				m_global_matrix = parent->m_global_matrix * compute_local_transform();
			}
			else
			{
				m_global_matrix = compute_local_transform();
			}

			is_dirty = false;
		}

	private:
		glm::vec3 m_position {0.0};
		glm::vec3 m_scale {1.0};
		glm::quat m_rotation = glm::identity<glm::quat>();
		glm::mat4 m_global_matrix {1.0};
	};
}
