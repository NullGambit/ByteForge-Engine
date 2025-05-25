#include "register_engine_components.hpp"

#include "camera_component.hpp"
#include "light_component.hpp"
#include "mesh_renderer_component.hpp"
#include "forge/core/engine.hpp"
#include "forge/ecs/ecs.hpp"

void forge::register_engine_components()
{
	auto *nexus = g_engine.get_subsystem<Nexus>();

	nexus->register_component<CameraComponent>();
	nexus->register_component<LightComponent>();
	nexus->register_component<MeshRendererComponent>();
	nexus->register_component<LightComponent>();
}
