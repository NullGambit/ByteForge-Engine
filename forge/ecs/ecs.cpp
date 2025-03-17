#include "ecs.hpp"

#include <mutex>
#include <sys/mman.h>

#include "forge/core/engine.hpp"

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

forge::TimerID forge::IComponent::add_timer(TimerOptions &&options) const
{
	auto &owner = m_owner->get_entity();

	return owner.get_nexus()->timer.add(std::forward<TimerOptions>(options));
}

void forge::IComponent::stop_timer(TimerID id) const
{
	auto &owner = m_owner->get_entity();

	owner.get_nexus()->timer.stop(id);
}

u8* forge::Entity::add_component(std::type_index index)
{
	return m_nexus->add_component(this, index);
}

std::vector<forge::EntityEntry>& forge::Entity::get_children()
{
	return m_nexus->m_entities_table[m_children_index].entities;
}

size_t forge::Entity::get_children_count()
{
	return get_children().size();
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
	for (auto &[_, component] : m_components)
	{
		component->on_editor_enter();
	}
}

void forge::Entity::update_hierarchy()
{
	if (m_parent != nullptr)
	{
		m_transform.m_model = m_parent->get_entity().m_transform.m_model * m_transform.compute_local_transform();
	}
	else
	{
		m_transform.m_model = m_transform.compute_local_transform();
	}

	on_transform_update(*this);

	if (m_children_index == 0)
	{
		return;
	}

	for (auto &[child, _] : get_children())
	{
		child.update_hierarchy();
	}
}

forge::EntityViewHandle forge::Entity::get_view() const
{
	return m_nexus->m_entities_table[m_table_index].entities[m_index].handle;
}

void forge::Entity::destroy()
{
	m_nexus->destroy_entity(this);
}

void forge::Entity::update_dirty_array() const
{
	std::scoped_lock lock {m_nexus->m_dirty_table_mutex};

	auto top_most_parent = get_top_most_parent();
	auto &entity = top_most_parent->get_entity();

	if (!entity.m_is_queued_for_cleaning)
	{
		m_nexus->m_entity_dirty_table.emplace_back(top_most_parent);
		entity.m_is_queued_for_cleaning = true;
	}
}

void forge::Nexus::ComponentType::free(IComponent *component)
{
	component->on_destroy();

	component->m_is_active = false;

	mem_pool.free(component->m_id);
}

void forge::Nexus::ComponentType::update(DeltaTime delta) const
{
	for (auto &component : mem_pool.get_iterator<IComponent>())
	{
		if (component.m_is_active && component.m_is_enabled) [[likely]]
		{
			component.update(delta);
		}
	}
}

std::string forge::Nexus::init(const EngineInitOptions &options)
{
	// construct global entities vector
	m_entities_table.emplace_back();

	return {};
}

void forge::Nexus::shutdown()
{
	// unregister all components so that their mempools will be destroyed
	for (auto &[type_index, _] : m_component_table)
	{
		unregister_component(type_index, false);
	}
}

void forge::Nexus::unregister_component(std::type_index type_index, bool remove_from_update_table)
{
	auto it = m_component_table.find(type_index);

	if (it == m_component_table.end())
	{
		return;
	}

	it->second.mem_pool.destroy();

	m_component_table.erase(it);

	if (remove_from_update_table)
	{
		// while this is a less than efficient way to remove the component type from the update table i think its
		// perfectly fine because unregistering a component will happen very rarely and likely never during gameplay
		// not worth the effort and cost to optimize
		auto index = std::find(m_update_table.begin(), m_update_table.end(), type_index);

		m_update_table.erase(index);
	}
}

forge::Entity & forge::Nexus::create_entity(std::string_view name)
{
	auto &entities = m_entities_table.front().entities;
	auto index = entities.size();
	auto &[entity, handle] = entities.emplace_back();

	entity.m_nexus = this;
	entity.m_table_index = 0;
	entity.m_index = index;
	entity.m_id = m_id_counter++;

	handle = entity.make_handle();

	// TODO: resolve name collisions
	if (!name.empty())
	{
		entity.m_name = name;
		m_name_table.emplace(entity.m_name, entity.get_view());
	}

	return entity;
}

void forge::Nexus::add_to_group(std::string_view group_name, Entity& entity)
{
	auto iter = m_groups.find(group_name);

	if (iter == m_groups.end())
	{
		iter = m_groups.emplace(group_name, std::vector<EntityViewHandle>{}).first;
	}

	iter->second.emplace_back(entity.get_view());
}

void forge::Nexus::remove_from_group(std::string_view group_name, Entity& entity)
{
	auto iter = m_groups.find(group_name);

	if (iter == m_groups.end())
	{
		return;
	}

	auto &entities = iter->second;

	for (auto i = 0; i < entities.size(); i++)
	{
		auto &view = entities[i];

		if (view->is_entity_valid() && view == entity.get_view())
		{
			std::swap(entities[i], entities[entities.size()-1]);
			entities.pop_back();
		}
	}
}

void forge::Nexus::remove_group(std::string_view group_name)
{
	m_groups.erase(group_name);
}

void forge::Nexus::create_group(std::string_view group_name)
{
	if (m_groups.contains(group_name))
	{
		return;
	}

	m_groups.emplace(group_name, std::vector<EntityViewHandle>{});
}

std::vector<forge::EntityViewHandle>* forge::Nexus::get_group(std::string_view group_name, bool trim_invalid_entities)
{
	auto iter = m_groups.find(group_name);

	if (iter == m_groups.end())
	{
		return nullptr;
	}

	auto &entities = iter->second;

	if (trim_invalid_entities)
	{
		for (auto i = 0; i < entities.size(); i++)
		{
			auto &view = entities[i];

			if (!view->is_entity_valid())
			{
				std::swap(entities[i], entities[entities.size()-1]);
				entities.pop_back();
			}
		}
	}

	return &entities;
}

forge::EntityViewHandle forge::Nexus::get_entity(std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return {};
	}

	return it->second;
}

void forge::Nexus::destroy_children(Entity* entity)
{
	if (entity->m_children_index == 0)
	{
		return;
	}

	auto &entry = m_entities_table[entity->m_children_index];

	for (auto &[child_entity, _]: entry.entities)
	{
		destroy_entity(&child_entity);
	}

	auto table_size = m_entities_table.size();

	// if it's at the end just pop it or if its just two values pop it so it doesn't do a weird swap with the top level table
	if ((entry.owner->table == table_size-1 && table_size > 1) || table_size == 2)
	{
		m_entities_table.pop_back();
		return;
	}

	auto old_index = entity->m_children_index;

	auto last_table = std::move(m_entities_table.back());

	m_entities_table[m_entities_table.size()-1] = std::move(entry);

	last_table.owner->get_entity().m_table_index = old_index;

	m_entities_table[old_index] = std::move(last_table);

	m_entities_table.pop_back();
}

void forge::Nexus::destroy_entity(Entity* entity)
{
	// free components and call their destructors
	for (auto &[index, component] : entity->m_components)
	{
		m_component_table[index].free(component);
	}

	destroy_children(entity);

	if (!entity->m_name.empty())
	{
		m_name_table.erase(entity->m_name);
	}

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

	entities[temp.entity.m_index].entity = std::move(*entity);

	temp.entity.m_index = old_index;
	temp.handle->index = old_index;

	entities[old_index] = std::move(temp);

	entities.pop_back();
}

u8* forge::Nexus::add_component(Entity *entity, std::type_index index)
{
	if (entity->m_components.contains(index))
	{
		return nullptr;
	}

	auto &ct = m_component_table[index];

	auto [ptr, id] = ct.mem_pool.allocate(true);

	auto *component = (IComponent*)ptr;

	component->m_owner = entity->get_view();
	component->m_is_active = true;
	component->m_is_enabled = true;
	component->m_id = id;

	component->on_create();

	if (auto reg_index = component->get_register_type(); reg_index != typeid(IComponent))
	{
		index = reg_index;
	}

	entity->m_components[index] = component;

	for (const auto &index : component->get_bundle())
	{
		add_component(entity, index);
	}

	return ptr;
}

forge::EcsResult forge::Nexus::remove_component(Entity *entity, std::type_index index)
{
	auto iter = entity->m_components.find(index);

	if (iter == entity->m_components.end())
	{
		return EcsResult::EntityDoesNotHaveComponent;
	}
	auto &ct = m_component_table[index];

	ct.free(iter->second);

	entity->m_components.erase(iter);

	return EcsResult::Ok;
}

void forge::Nexus::update()
{
	auto delta = g_engine.get_delta();

	for (auto &type : m_update_table)
	{
		auto iter = m_component_table.find(type);

		if (iter == m_component_table.end())
		{
			continue;
		}

		iter->second.update(delta);
	}

	if (!m_entity_dirty_table.empty())
	{
		std::scoped_lock lock {m_dirty_table_mutex};

		for (auto &handle : m_entity_dirty_table)
		{
			auto &entity = handle->get_entity();
			entity.update_hierarchy();
			entity.m_is_queued_for_cleaning = false;
		}

		m_entity_dirty_table.clear();
	}
}

void forge::Nexus::clear()
{
	for (auto &[_, ct] : m_component_table)
	{
		ct.mem_pool.reset(true, [](u8 *ptr)
		{
			auto *component = (IComponent*)ptr;

			if (component->m_is_active)
			{
				component->m_is_active = false;
				component->on_destroy();
			}
		});
	}

	m_entity_dirty_table.clear();
	m_groups.clear();
	// m_update_table.clear();
	m_remove_queue.clear();
	m_name_table.clear();
	// m_component_table.clear();

	for (auto &[handle, entities] : m_entities_table)
	{
		entities.clear();
		handle.reset();
	}
}
