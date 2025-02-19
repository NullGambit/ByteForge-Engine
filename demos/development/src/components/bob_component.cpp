#include "bob_component.hpp"

void BobComponent::update(forge::DeltaTime delta)
{
	m_accumulated_time += delta;

	auto &owner = m_owner->get_entity();

	auto position = owner.get_local_position();

	auto angular_frequency = 2.0f * M_PI * frequency;

	auto sin_bob = amplitude * glm::sin(angular_frequency * m_accumulated_time);

	if (bob_x)
	{
		position.x = sin_bob;
	}
	if (bob_z)
	{
		position.z = sin_bob;
	}
	if (bob_y)
	{
		position.y = amplitude * glm::cos(angular_frequency * m_accumulated_time);
	}

	owner.set_local_position(position);
}