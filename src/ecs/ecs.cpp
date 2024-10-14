#include "ecs.hpp"

#include <sys/mman.h>

#include "core/engine.hpp"

forge::Entity& forge::EntityView::get()
{
	return nexus->m_entities[table][index];
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
	return m_nexus->m_entities[m_children_index];
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

forge::Entity& forge::Entity::emplace_child(std::optional<std::string_view> name)
{
	auto should_create_children = m_children_index + 1 >= m_nexus->m_entities.size();
	auto &children = should_create_children ? m_nexus->m_entities.emplace_back() : m_nexus->m_entities[m_children_index];
	auto index = children.size();
	auto &entity = children.emplace_back();

	if (should_create_children)
	{
		m_children_index = m_nexus->m_entities.size() - 1;
	}

	entity.m_nexus = m_nexus;
	entity.m_parent = get_view();
	entity.m_index = index;

	if (name)
	{
		entity.m_name = name.value();
		m_nexus->m_name_table[entity.m_name] = entity.get_view();
	}

	return entity;
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
	// if (m_parent.has_value())
	// {
	// 	m_transform.model = m_parent->m_transform.model * m_transform.get_local_transform();
	// }
	// else
	// {
	// 	m_transform.model = m_transform.get_local_transform();
	// }
	//
	// for (auto &child : m_children)
	// {
	// 	child.update_hierarchy();
	// }
}

void forge::Entity::destroy()
{
	m_nexus->destroy_entity(this);
}

u32 forge::Entity::get_entity_table()
{
	return !m_parent.has_value() ? 0 : m_nexus->m_entities[m_parent.table][m_parent.index].m_children_index;
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
	m_entities.emplace_back();

	return {};
}

void forge::Nexus::shutdown()
{
}

forge::Entity* forge::Nexus::get_entity(std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return nullptr;
	}

	return &it->second.get();
}

void forge::Nexus::destroy_entity(Entity* entity)
{
	// free components and call their destructors
	for (auto &[index, cv] : entity->m_components)
	{
		m_component_table[index].free(cv.offset);
	}

	// swaps the current entity with the last entity in the table and updates its index then pops it for a fast removal

	auto old_index = entity->m_index;

	auto table_index = entity->get_entity_table();
	auto &table = m_entities[table_index];

	if (table.size() == 1)
	{
		table.pop_back();
		return;
	}
	if (table.empty())
	{
		return;
	}

	auto temp = std::move(table.back());

	table[temp.m_index] = std::move(*entity);

	temp.m_index = old_index;

	table[old_index] = std::move(temp);

	table.pop_back();
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

	for (auto &table : m_entities)
	{
		for (auto &entity : table)
		{
			entity.update_hierarchy();
		}
	}
}
