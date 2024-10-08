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

u8* forge::Entity::add_component(std::type_index index)
{
	return m_nexus->add_component(this, index);
}

void forge::Entity::set_name(std::string_view new_name)
{
	if (!m_name.empty())
	{
		m_nexus->m_name_table.erase(m_name);
	}

	m_name = new_name;

	auto *table = get_entity_table();

	m_nexus->m_name_table[m_name] = EntityView{m_index, table};
}

forge::Entity& forge::Entity::emplace_child(std::optional<std::string_view> name)
{
	auto index = m_children.size();
	auto &entity = m_children.emplace_back();

	entity.m_nexus = m_nexus;
	entity.m_parent = this;
	entity.m_index = index;

	if (name)
	{
		entity.m_name = name.value();
		m_nexus->m_name_table[entity.m_name] = EntityView{(u32)index, &m_children};
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

std::vector<forge::Entity>* forge::Entity::get_entity_table()
{
	return m_parent == nullptr ? &m_nexus->m_entities : &m_parent->m_children;
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

	return &it->second.get();
}

void forge::Nexus::delete_entity(Entity* entity)
{
	entity->m_state = EntityState::Invalid;

	for (auto &[index, cv] : entity->m_components)
	{
		m_component_table[index].mem_pool.free(cv.offset);
	}

	entity->m_components.clear();
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

	for (auto &entity : m_entities)
	{
		entity.update_hierarchy();
	}
}
