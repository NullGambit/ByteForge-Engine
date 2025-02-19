#pragma once
#include "forge/ecs/ecs.hpp"

class BobComponent : public forge::IComponent
{
public:
	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override;

	EXPORT_FIELDS(&frequency, &amplitude, &bob_x, &bob_y, &bob_z);

	float frequency = 0.5;
	float amplitude = 0.5;

	bool bob_x = false;
	bool bob_y = false;
	bool bob_z = true;
private:
	forge::DeltaTime m_accumulated_time {};
};