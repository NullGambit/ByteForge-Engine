#include "mesh_loading.hpp"

#include "components/light_component.hpp"
#include "components/mesh_renderer_component.hpp"
#include "forge/core/engine.hpp"
#include "forge/ecs/ecs.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	void create_entity_from_object(RenderObjectTree &node, Entity *parent)
	{
		auto &entity = parent ? parent->emplace_child(node.name) : g_engine.nexus->create_entity(node.name);

		if (node.light)
		{
			auto *light_comp = entity.add_component<LightComponent>();

			auto *light = light_comp->get_light();

			*light = node.light.value();
			light->enabled = true;
			light->is_available = false;
		}

		if (node.object)
		{
			auto *mesh = entity.add_component<MeshRendererComponent>();

			mesh->set_mesh(node);
		}

		for (auto &child : node.children)
		{
			create_entity_from_object(child, &entity);
		}
	}
}

void forge::load_meshes_hierarchy(std::string_view filepath, MeshLoadOptions options)
{
	auto tree = g_engine.renderer->create_render_object(filepath, options);

	create_entity_from_object(tree, nullptr);
}
