#include "register_engine_components.hpp"

#include "camera_component.hpp"
#include "forge/ecs/ecs.hpp"

void forge::register_engine_components(Nexus *nexus)
{
	nexus->register_component<CameraComponent>();
}
