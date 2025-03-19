#pragma once
#include "forge/ecs/ecs.hpp"

struct LifetimeComponent : forge::IComponent
{
	forge::TimerDuration duration {};

	void start();

	forge::ButtonField start_button {"Start timer", [&]
	{
		start();
	}};

	EXPORT_FIELDS(&duration, start_button);
};