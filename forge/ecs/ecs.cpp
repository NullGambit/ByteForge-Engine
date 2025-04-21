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
	return m_owner->get_nexus()->timer.add(std::forward<TimerOptions>(options));
}

void forge::IComponent::stop_timer(TimerID id) const
{
	m_owner->get_nexus()->timer.stop(id);
}

u8* forge::Entity::add_component(std::type_index index)
{
	return m_nexus->add_component(this, index);
}

forge::VirtualArray<forge::Entity> forge::Entity::get_children() const
{
	return m_children;
}

size_t forge::Entity::get_children_count() const
{
	return m_children.get_length();
}

void forge::Entity::set_name(std::string_view new_name)
{
	if (!m_name.empty())
	{
		m_nexus->m_name_table.erase(m_name);
	}

	m_name = new_name;

	m_nexus->m_name_table[m_name] = this;
}

void forge::Entity::remove_component(std::type_index index)
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
		m_transform.m_global_matrix = m_parent->m_transform.m_global_matrix * m_transform.compute_local_transform();
	}
	else
	{
		m_transform.m_global_matrix = m_transform.compute_local_transform();
	}

	for (auto &child : m_children)
	{
		child.update_hierarchy();
	}

	on_transform_update(*this);
}

void forge::Entity::destroy()
{
	m_nexus->destroy_entity(this);
}

void forge::Entity::update_dirty_array()
{
	std::scoped_lock lock {m_nexus->m_dirty_table_mutex};

	auto top_most_parent = get_top_most_parent();

	if (!top_most_parent->m_is_queued_for_cleaning)
	{
		m_nexus->m_entity_dirty_table.emplace_back(top_most_parent);
		top_most_parent->m_is_queued_for_cleaning = true;
	}
}

void forge::Entity::set_parent(Entity *parent)
{
	m_parent = parent;
	m_transform.m_parent = &parent->m_transform;
}

void forge::Nexus::ComponentType::free(IComponent *component)
{
	component->on_destroy();

	component->m_is_valid = false;

	mem_pool.free(component->m_id);
}

void forge::Nexus::ComponentType::update(DeltaTime delta) const
{
	for (auto &component : mem_pool.get_iterator<IComponent>())
	{
		if (component.m_is_valid && component.m_is_enabled) [[likely]]
		{
			component.update(delta);
		}
	}
}

std::string forge::Nexus::init(const EngineInitOptions &options)
{
	m_entities.init(ECS_ENTITY_POOL_SIZE);

	return {};
}

void forge::Nexus::shutdown()
{
	// unregister all components so that their mempools will be destroyed
	for (auto &[type_index, _] : m_component_table)
	{
		unregister_component(type_index, false);
	}

	// call destructors so allocated memory will be freed
	m_entities.clear();
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

forge::Entity* forge::Nexus::create_entity(std::string_view name, Entity *parent)
{
	const auto is_child = parent != nullptr;

	if (is_child && !parent->m_children.is_initialized())
	{
		parent->m_children.init(ECS_CHILD_LIMIT);
	}

	auto [entity, reused] = is_child ? parent->m_children.allocate() : m_entities.allocate();

	if (!reused)
	{
		new (entity) Entity();
	}

	if (is_child)
	{
		entity->set_parent(parent);
		entity->m_top_most_parent = parent->get_top_most_parent();
	}

	entity->m_nexus = this;
	entity->m_id = m_id_counter++;
	entity->m_is_valid = true;

	if (!name.empty())
	{
		entity->m_name = name;
		m_name_table.emplace(entity->m_name, entity);
	}

	return entity;
}

void forge::Nexus::add_to_group(std::string_view group_name, Entity *entity)
{
	if (entity == nullptr)
	{
		return;
	}

	auto iter = m_groups.find(group_name);

	if (iter == m_groups.end())
	{
		iter = m_groups.emplace(group_name, Array<Entity*>{}).first;
	}

	iter->second.emplace_back(entity);
}

void forge::Nexus::remove_from_group(std::string_view group_name, Entity *entity)
{
	auto iter = m_groups.find(group_name);

	if (iter == m_groups.end() || entity == nullptr)
	{
		return;
	}

	auto &entities = iter->second;

	for (auto i = 0; i < entities.size(); i++)
	{
		auto *other = entities[i];

		if (other->is_valid() && other->get_id() == entity->get_id())
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

	m_groups.emplace(group_name, std::vector<Entity*>{});
}

std::vector<forge::Entity*>* forge::Nexus::get_group(std::string_view group_name, bool trim_invalid_entities)
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

			if (!view->is_valid())
			{
				std::swap(entities[i], entities[entities.size()-1]);
				entities.pop_back();
			}
		}
	}

	return &entities;
}

forge::Entity* forge::Nexus::get_entity(std::string_view name)
{
	auto it = m_name_table.find(name);

	if (it == m_name_table.end())
	{
		return {};
	}

	return it->second;
}

void forge::Nexus::destroy_entity(Entity *entity)
{
	for (auto &[index, component] : entity->m_components)
	{
		m_component_table[index].free(component);
	}

	entity->m_components.clear();

	for (auto &child : entity->m_children)
	{
		destroy_entity(&child);
	}

	if (!entity->m_name.empty())
	{
		m_name_table.erase(entity->m_name);
	}

	entity->m_is_valid = false;
	entity->m_children.clear(false);
	entity->m_transform = {};
	entity->m_name.clear();
	entity->m_parent = nullptr;
	entity->m_top_most_parent = nullptr;

	m_entities.free(entity, false);
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

	component->m_owner = entity;
	component->m_is_valid = true;
	component->m_is_enabled = true;
	component->m_id = id;

	for (const auto &index : component->get_bundle())
	{
		add_component(entity, index);
	}

	component->on_create();

	if (auto reg_index = component->get_register_type(); reg_index != typeid(IComponent))
	{
		index = reg_index;
	}

	entity->m_components[index] = component;

	return ptr;
}

void forge::Nexus::remove_component(Entity *entity, std::type_index index)
{
	auto iter = entity->m_components.find(index);

	if (iter == entity->m_components.end())
	{
		return;
	}

	auto &ct = m_component_table[index];

	ct.free(iter->second);

	entity->m_components.erase(iter);
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

	timer.process();

	if (!m_entity_dirty_table.empty())
	{
		std::scoped_lock lock {m_dirty_table_mutex};

		for (auto *entity : m_entity_dirty_table)
		{
			entity->update_hierarchy();
			entity->m_is_queued_for_cleaning = false;
		}

		m_entity_dirty_table.clear();
	}
}

forge::VirtualArray<forge::Entity> forge::Nexus::get_entities()
{
	return m_entities;
}

void forge::Nexus::clear()
{
	for (auto &[_, ct] : m_component_table)
	{
		ct.mem_pool.reset(true, [](u8 *ptr)
		{
			auto *component = (IComponent*)ptr;

			if (component->m_is_valid)
			{
				component->m_is_valid = false;
				component->on_destroy();
			}
		});
	}

	m_entity_dirty_table.clear();
	m_groups.clear();
	// m_update_table.clear();
	m_name_table.clear();
	// m_component_table.clear();

	m_entities.clear(false);
}
