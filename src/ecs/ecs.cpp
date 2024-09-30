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

void forge::Entity::remove_child(Entity *child)
{
	auto *node = m_children;

	while (node->m_siblings != child)
	{
		node = node->m_siblings;
	}

	if (node != nullptr && node->m_siblings != nullptr)
	{
		node->m_siblings = node->m_siblings->m_siblings;
	}
}

void forge::Entity::add_child(Entity *child)
{
	if (child->m_parent != nullptr)
	{
		child->m_parent->remove_child(child);
	}

	child->m_parent = this;

	if (m_children == nullptr)
	{
		m_children = child;
		return;
	}

	auto *node = m_children->m_siblings;

	if (node == nullptr)
	{
		m_children->m_siblings = child;
		return;
	}

	while (node->m_siblings != nullptr)
	{
		node = node->m_siblings;
	}

	node->m_siblings = child;
}

void forge::Entity::foreach_child(std::function<void(Entity*)> callback, bool recursive, Entity *root)
{
	Entity *node;

	if (root == nullptr)
	{
		node = m_children;
	}
	else
	{
		node = root->m_children;
	}

	while (node != nullptr)
	{
		callback(node);

		node = node->get_sibling();

		if (node != nullptr && recursive)
		{
			foreach_child(callback, recursive, node);
		}
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
	m_entities.init(sizeof(Entity), ECS_MAX_MAPPED_MEMORY);

	return {};
}

// TODO: handle shutdown
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

	// TODO: make sure constructor is called
	for (auto &[index, cv] : entity->m_components)
	{
		m_component_table[index].mem_pool.free(cv.offset);
	}

	entity->m_components.clear();

	m_entities.free(entity->m_offset);
}

void forge::Nexus::update()
{
	auto current_time = Engine::get_instance().get_engine_runtime();

	auto delta = current_time - m_previous_time;

	m_previous_time = current_time;

	for (auto &type : m_update_table)
	{
		auto iter = m_component_table.find(type);

		if (iter == m_component_table.end())
		{
			continue;
		}

		iter->second.update(delta);
	}
}
