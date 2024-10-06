#include "ecs.hpp"

#include <sys/mman.h>

#include "core/engine.hpp"

void forge::IComponent::set_enabled(bool value)
{
	m_is_enabled = value;

	if (m_is_enabled)
	{
		on_enabled();
	}
	else
	{
		on_disabled();
	}
}

void forge::Entity::on_editor_enter()
{
	for (auto &[_, view] : m_components)
	{
		auto *component = (IComponent*)view.pointer;

		component->on_editor_enter();
	}
}

void forge::Entity::update_hierarchy()
{
	if (m_parent)
	{
		m_transform.model = m_parent->m_transform.model * m_transform.get_local_transform();
	}
	else
	{
		m_transform.model = m_transform.get_local_transform();
	}

	for (auto &child : m_children)
	{
		child.update_hierarchy();
	}
}

void forge::Nexus::ComponentType::update(DeltaTime delta) const
{
	auto *memory = mem_pool.memory();

	for (int i = 0; i < mem_pool.length(); i++)
	{
		auto *component = (IComponent*)memory;

		if (component->m_is_active && component->m_is_enabled)
		{
			component->update(delta);
		}

		memory += mem_pool.element_size();
	}
}

std::string forge::Nexus::init()
{
	return {};
}

void forge::Nexus::shutdown()
{
}

forge::Entity* forge::Nexus::get_entity(const std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return nullptr;
	}

	return it->second;
}

void forge::Nexus::delete_entity(Entity* entity)
{
	if (!is_entity_valid(entity))
	{
		return;
	}

	entity->m_state = EntityState::Invalid;

	for (auto &[index, cv] : entity->m_components)
	{
		m_component_table[index].mem_pool.free(cv.offset);
	}

	entity->m_components.clear();
}

void forge::Nexus::update()
{
	auto &engine = Engine::get_instance();

	auto delta = engine.get_delta();

	for (auto &type : m_update_table)
	{
		auto iter = m_component_table.find(type);

		if (iter == m_component_table.end())
		{
			continue;
		}

		iter->second.update(delta);
	}

	for (auto &entity : m_entities)
	{
		entity.update_hierarchy();
	}
}
