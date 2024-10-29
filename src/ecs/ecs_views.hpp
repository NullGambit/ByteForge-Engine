#pragma once
#include <limits>
#include <memory>

namespace forge
{
	using EntityID = u32;

	class Nexus;
	class Entity;

	struct ComponentView
	{
		size_t offset;
		u8 *pointer;
	};

	struct EntityView
	{
		static constexpr auto NO_INDEX = std::numeric_limits<u32>::max();

		Nexus *nexus;
		u32 index = NO_INDEX;
		u32 table;
		EntityID id;

		Entity& get_entity();

		// verifies that this entity points to the same entity the view was taken from and that it still exists in the table
		[[nodiscard]]
		bool is_entity_valid();

		// equivalent to doing a null check on an entity pointer
		[[nodiscard]]
		inline bool has_value() const
		{
			return index != NO_INDEX;
		}

		bool operator==(const EntityView &b) const
		{
			return index == b.index && table == b.table && id == b.id;
		}
	};

	using EntityViewHandle = std::shared_ptr<EntityView>;
}
