#include "lifetime_component.hpp"

void LifetimeComponent::start()
{
	add_timer(
	{
		.duration = duration,
		.on_timeout = [&owner = m_owner]
		{
			log::info("destroyed {}", owner->get_name());
			owner->destroy();
		},
		.one_shot = true
	});
}
