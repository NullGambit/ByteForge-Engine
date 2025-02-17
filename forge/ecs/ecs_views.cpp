#include "ecs_views.hpp"

#include "ecs.hpp"

forge::Entity& forge::EntityView::get_entity()
{
	return nexus->m_entities_table[table].entities[index].entity;
}

bool forge::EntityView::is_entity_valid()
{
	auto &entities_table = nexus->m_entities_table;

	if (table >= entities_table.size() || index >= entities_table[table].entities.size())
	{
		return false;
	}

	return id == get_entity().m_id;
}
