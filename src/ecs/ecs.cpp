#include "ecs.hpp"

#include <sys/mman.h>

void ecs::Entity::remove_child(Entity *child)
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

void ecs::Entity::add_child(Entity *child)
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

void ecs::Entity::foreach_child(std::function<void(Entity*)> callback, bool recursive, Entity *root)
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
void ecs::Nexus::ComponentType::update(DeltaTime delta) const
{
	auto *memory = mem_pool.memory();

	for (int i = 0; i < mem_pool.length(); i++)
	{
		auto *component = (BaseComponent*)memory;

		if (component->m_is_active)
		{
			component->update(delta);
		}

		memory += mem_pool.element_size();
	}
}

ecs::Entity* ecs::Nexus::get_entity(const std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return nullptr;
	}

	return it->second;
}

void ecs::Nexus::delete_entity(Entity* entity)
{
	if (!is_entity_valid(entity))
	{
		return;
	}

	entity->m_state = EntityState::Invalid;

	// TODO: make sure constructor is called
	for (auto &[index, cv] : entity->m_components)
	{
		m_type_table[index].mem_pool.free(cv.offset);
	}

	entity->m_components.clear();

	m_entities.free(entity->m_offset);
}
