#include "ecs.hpp"

#include <sys/mman.h>

#include "core/engine.hpp"

forge::Entity& forge::EntityView::get()
{
	return nexus->m_entities_table[table].entities[index];
}

bool forge::EntityView::is_entity_valid()
{
	auto &entities_table = nexus->m_entities_table;

	if (table >= entities_table.size() || index >= entities_table[table].entities.size())
	{
		return false;
	}

	return id == get().m_id;
}

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

u8* forge::Entity::add_component(std::type_index index)
{
	return m_nexus->add_component(this, index);
}

std::vector<forge::Entity>& forge::Entity::get_children()
{
	return m_nexus->m_entities_table[m_children_index].entities;
}

void forge::Entity::set_name(std::string_view new_name)
{
	if (!m_name.empty())
	{
		m_nexus->m_name_table.erase(m_name);
	}

	m_name = new_name;

	m_nexus->m_name_table[m_name] = get_view();
}

forge::EcsResult forge::Entity::remove_component(std::type_index index)
{
	return m_nexus->remove_component(this, index);
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
	if (m_parent.has_value())
	{
		m_transform.model = m_parent.get().m_transform.model * m_transform.get_local_transform();
	}
	else
	{
		m_transform.model = m_transform.get_local_transform();
	}

	if (m_children_index == 0)
	{
		return;
	}

	for (auto &child : get_children())
	{
		child.update_hierarchy();
	}
}

void forge::Entity::destroy()
{
	m_nexus->destroy_entity(this);
}

void forge::Nexus::ComponentType::update(DeltaTime delta) const
{
	auto *memory = mem_pool.get_memory();

	for (int i = 0; i < mem_pool.get_length(); i++)
	{
		auto *component = (IComponent*)memory;

		if (component->m_is_active && component->m_is_enabled)
		{
			component->update(delta);
		}

		memory += mem_pool.get_element_size();
	}
}

std::string forge::Nexus::init()
{
	// m_entities.reserve(1000);

	// construct global entities vector
	m_entities_table.emplace_back();

	return {};
}

void forge::Nexus::shutdown()
{
}

forge::EntityView forge::Nexus::get_entity(std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return {};
	}

	return it->second.get().get_view();
}

void forge::Nexus::destroy_children(Entity* entity)
{
	if (entity->m_children_index == 0)
	{
		return;
	}

	auto &entry = m_entities_table[entity->m_children_index];

	for (auto &child_entity : entry.entities)
	{
		destroy_entity(&child_entity);
	}

	auto table_size = m_entities_table.size();

	// if it's at the end just pop it or if its just two values pop it so it doesnt do a weird swap with the top level table
	if ((entry.owner.table == table_size-1 && table_size > 1) || table_size == 2)
	{
		m_entities_table.pop_back();
		return;
	}

	auto old_index = entity->m_children_index;

	auto last_table = std::move(m_entities_table.back());

	m_entities_table[m_entities_table.size()-1] = std::move(entry);

	last_table.owner.get().m_table_index = old_index;

	m_entities_table[old_index] = std::move(last_table);

	m_entities_table.pop_back();
}

void forge::Nexus::destroy_entity(Entity* entity)
{
	// free components and call their destructors
	for (auto &[index, cv] : entity->m_components)
	{
		m_component_table[index].free(cv.offset);
	}

	destroy_children(entity);

	// swaps the current entity with the last entity in the table and updates its index then pops it for a fast removal

	auto old_index = entity->m_index;

	auto table_index = entity->m_table_index;
	auto &entities = m_entities_table[table_index].entities;

	// if its already at the back just pop it and stop
	if (entities.size() == 1)
	{
		entities.pop_back();
		return;
	}
	if (entities.empty())
	{
		return;
	}

	auto temp = std::move(entities.back());

	entities[temp.m_index] = std::move(*entity);

	temp.m_index = old_index;

	entities[old_index] = std::move(temp);

	entities.pop_back();
}

u8* forge::Nexus::add_component(Entity* entity, std::type_index index)
{
	auto &ct = m_component_table[index];

	auto [ptr, offset] = ct.mem_pool.allocate(true);

	if (ct.is_component)
	{
		auto *component = (IComponent*)ptr;
		component->m_owner = entity->get_view();
		component->m_is_active = true;
		component->m_is_enabled = true;
	}

	entity->m_components[index] =
	{
		.offset =  offset,
		.pointer = ptr
	};

	return ptr;
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

	for (auto &table : m_entities_table)
	{
		for (auto &entity : table.entities)
		{
			entity.update_hierarchy();
		}
	}
}
