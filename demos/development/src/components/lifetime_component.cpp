#include "lifetime_component.hpp"

void LifetimeComponent::start()
{
	add_timer(
	{
		.duration = duration,
		.on_timeout = [&owner = m_owner]
		{
			log::info("destroyed");
			owner->get_entity().destroy();
		},
		.one_shot = true
	});
}
