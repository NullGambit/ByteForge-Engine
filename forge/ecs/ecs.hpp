#pragma once

#include <cstdint>
#include <typeindex>
#include <concepts>
#include <variant>
#include <tuple>

#include "forge/container/map.hpp"
#include "forge/fmt/fmt.hpp"
#include "forge/memory/defs.hpp"
#include "forge/memory/mem_pool.hpp"
#include "forge/events/signal.hpp"
#include "forge/core/isub_system.hpp"
#include "defs.hpp"

#include "component_field.hpp"
#include "ecs_views.hpp"
#include "transform.hpp"
#include "forge/util/types.hpp"

// should get included as a part of ecs.hpp. don't remove this.
#include "macro_warcrimes.hpp"
#include "forge/container/array.hpp"
#include "forge/events/timer.hpp"

// the maximum amount of virtual memory that will be used for each component by default unless specified otherwise by the component
#define DEFAULT_ECS_MAX_MAPPED_MEMORY MB(48)

namespace forge
{
    enum class EcsResult
    {
        Ok,
        ComponentAlreadyAddedToEntity,
        ComponentNotRegistered,
        ComponentAlreadyRegistered,
        CouldNotAllocateComponentMemory,
        CouldNotAddComponentToTypeMap,
        EntityDoesNotHaveComponent,
        EntityDoesNotExist,
    };

    struct EntityEntry;

    struct EntitiesTableEntry
    {
        EntityViewHandle owner;
        std::vector<EntityEntry> entities;
    };

    using OnComponentDestroy = Signal<void()>;

    class IComponent
    {
    public:
        virtual ~IComponent() = default;

        virtual void update(DeltaTime delta) {}

        void set_enabled(bool value);

        [[nodiscard]]
        bool is_enabled() const
        {
            return m_is_enabled;
        }

        TimerID add_timer(TimerOptions &&options) const;
        void stop_timer(TimerID id) const;

        virtual Array<std::type_index> get_bundle() { return {}; }

        virtual Array<ComponentField> export_fields() { return {}; }

        // gets the type this component should be registered as useful for ensuring derived classes
        // will always get registered as its base interface
        virtual std::type_index get_register_type() { return typeid(IComponent); }

        virtual void on_editor_enter() {}

        [[nodiscard]]
        u32 get_id() const
        {
            return m_id;
        }

    private:
        friend Nexus;
        // if false this component has been freed
        bool m_is_active = true;
        // if false this component should not be updated
        bool m_is_enabled = true;

        u32 m_id;

    protected:
        EntityViewHandle m_owner;

        virtual void on_enabled() {}
        virtual void on_disabled() {}
        virtual void on_create() {}
        virtual void on_destroy() {}
    };

    class Entity final
    {
    public:

        Signal<void(Entity&)> on_transform_update;

        template<class T>
        T* add_component();

        u8* add_component(std::type_index index);

        template<class ...Args>
        void add_components();

        template<class T>
        T* get_component();

        std::vector<EntityEntry>& get_children();

        [[nodiscard]]
        inline u32 get_children_index() const
        {
            return m_children_index;
        }

        [[nodiscard]]
        inline bool has_children() const
        {
            return m_children_index > 0;
        }

        [[nodiscard]]
        size_t get_children_count();

        [[nodiscard]]
        inline EntityID get_id() const
        {
            return m_id;
        }

        [[nodiscard]]
        inline Transform& get_transform()
        {
            return m_transform;
        }

        inline void set_transform(const Transform &transform)
        {
            m_transform = transform;
        }

        [[nodiscard]]
        inline std::string_view get_name() const
        {
            return m_name;
        }

        [[nodiscard]]
        inline Nexus* get_nexus() const
        {
            return m_nexus;
        }

        void set_name(std::string_view new_name);

        [[nodiscard]]
        inline const HashMap<std::type_index, IComponent*>& get_components() const
        {
            return m_components;
        }

        template<class ...Args>
        Entity& emplace_child(std::optional<std::string_view> name = std::nullopt);

        template<class T>
        EcsResult remove_component();

        EcsResult remove_component(std::type_index index);

        void on_editor_enter();

        void update_hierarchy();

        [[nodiscard]]
        EntityViewHandle get_view() const;

        void destroy();

        [[nodiscard]]
        inline bool has_parent() const
        {
            return m_parent != nullptr;
        }

        [[nodiscard]]
        inline EntityViewHandle get_parent() const
        {
            return m_parent;
        }

        [[nodiscard]]
        inline EntityViewHandle get_top_most_parent() const
        {
            if (m_parent == nullptr)
            {
                return get_view();
            }

            return m_top_most_parent;
        }

        [[nodiscard]]
        inline const Transform& get_top_most_parent_transform() const
        {
            return get_top_most_parent()->get_entity().m_transform;
        }

        [[nodiscard]]
        inline Transform get_top_most_parent_transform()
        {
            return get_top_most_parent()->get_entity().m_transform;
        }

        inline void set_local_position(Transform::PositionT position)
        {
            update_dirty_array();
            m_transform.set_local_position(position);
        }

        inline Transform::PositionT get_local_position() const
        {
            return m_transform.m_position;
        }

        inline void set_local_rotation(glm::vec3 euler_rotation)
        {
            update_dirty_array();
            m_transform.set_local_rotation(
                {   glm::radians(euler_rotation.x),
                    glm::radians(euler_rotation.y),
                    glm::radians(euler_rotation.z)});
        }

        inline void set_local_rotation(glm::quat rotation)
        {
            update_dirty_array();
            m_transform.set_local_rotation(rotation);
        }

        inline glm::quat get_local_rotation() const
        {
            return m_transform.m_rotation;
        }

        inline glm::vec3 get_local_euler_rotation() const
        {
            return m_transform.get_local_euler_rotation();
        }

        inline void set_local_scale(glm::vec3 scale)
        {
            update_dirty_array();
            m_transform.set_local_scale(scale);
        }

        inline glm::vec3 get_local_scale() const
        {
            return m_transform.m_scale;
        }

        inline void set_position(Transform::PositionT position)
        {
            update_dirty_array();
            m_transform.set_position(get_top_most_parent_transform().m_position, position);
        }

        inline Transform::PositionT get_position() const
        {
            return m_transform.get_position(get_top_most_parent_transform().m_position);
        }

        inline void set_rotation(glm::vec3 euler_rotation)
        {
            update_dirty_array();
            m_transform.set_rotation(get_top_most_parent_transform().m_rotation, euler_rotation);
        }

        inline void set_rotation(glm::quat rotation)
        {
            update_dirty_array();
            m_transform.set_rotation(get_top_most_parent_transform().m_rotation, rotation);
        }

        inline glm::quat get_rotation() const
        {
            return m_transform.get_rotation(get_top_most_parent_transform().m_rotation);
        }

        inline glm::vec3 get_euler_rotation() const
        {
            return m_transform.get_euler_rotation(get_top_most_parent_transform().m_rotation);
        }

        inline void set_scale(glm::vec3 scale)
        {
            update_dirty_array();
            m_transform.set_rotation(get_top_most_parent_transform().m_scale, scale);
        }

        inline glm::vec3 get_scale() const
        {
            return m_transform.get_euler_rotation(get_top_most_parent_transform().m_scale);
        }

        inline Transform::MatT get_model() const
        {
            return m_transform.m_model;
        }

    private:
        friend Nexus;
        friend EntityView;

        Transform m_transform;

        std::string m_name;

        HashMap<std::type_index, IComponent*> m_components;

        Nexus *m_nexus = nullptr;

        // a view to its parent if it has any
        EntityViewHandle m_parent = nullptr;
        // the highest parent in this hierarchy at the top level in table 0
        // this is mostly for global transform calculations and for the entity dirty table
        EntityViewHandle m_top_most_parent = nullptr;
        // the index of where its child is stored in the nexus' entity table
        u32 m_children_index = 0;
        // the index of where this entity is stored within its table
        u32 m_index;
        // the actual table this entity belongs to if 0 than it is a top level entity
        u32 m_table_index;
        // an id used for comparison and verification to make sure the entity has not been changed (swapped and popped?)
        EntityID m_id;

        // will be set to true if any transform component is marked dirty and signifies that this entity has been marked for cleaning
        // is mainly used to avoid trying to add it to the dirty table more than once since the table is a vector
        bool m_is_queued_for_cleaning = false;

        [[nodiscard]]
        EntityViewHandle make_handle() const
        {
            return std::make_shared<EntityView>(EntityView
            {
                m_nexus,
                m_index,
                m_table_index,
                m_id
            });
        }

        void update_dirty_array() const;
    };

    struct EntityEntry
    {
        // the underlying entity
        Entity entity;
        // a view to the entity that will be given out as a reference and always kept updated
        EntityViewHandle handle;
    };

    // if used within an IComponent class it will be used as a tag to signify that this component should be placed within the update table
    // the actual function itself is not called ever
#define REGISTER_UPDATE_FUNC void __should_ever_update__() {}
#define SET_MAX_COMPONENT_MEMORY(amount) constexpr static size_t max_component_memory() { return (amount); }

    template<class T>
    concept ComponentShouldEverUpdate = requires(T t)
    {
        t.__should_ever_update__();
    };

    class Nexus final : public ISubSystem
    {
        struct ComponentType
        {
            MemPool mem_pool;

            void free(IComponent *component);

            void update(DeltaTime delta) const;
        };

    public:

        std::string init(const EngineInitOptions &options) override;

        void shutdown() override;

        bool is_critical() override { return true; }

        template<class T>
        requires(std::derived_from<T, IComponent>)
        EcsResult register_component(bool override_should_update = false)
        {
            constexpr auto &type = typeid(T);

            if (m_component_table.contains(type))
            {
                return EcsResult::ComponentAlreadyRegistered;
            }

            auto emplaced = m_component_table.emplace(type, ComponentType{});

            auto &ct = emplaced.first->second;

            auto component_size = DEFAULT_ECS_MAX_MAPPED_MEMORY;

            if constexpr (requires { T::max_component_memory; })
            {
                component_size = T::max_component_memory();
            }

            auto result = ct.mem_pool.init<T>(component_size);

            if (!result)
            {
                return EcsResult::CouldNotAllocateComponentMemory;
            }

            if (ComponentShouldEverUpdate<T> || override_should_update)
            {
                m_update_table.emplace_back(type);
            }

            return emplaced.second ? EcsResult::Ok : EcsResult::CouldNotAddComponentToTypeMap;
        }

        template<class T>
        void unregister_component(bool remove_from_update_table = true)
        {
            unregister_component(typeid(T), remove_from_update_table);
        }

        void unregister_component(std::type_index type_index, bool remove_from_update_table = true);

        template<class T>
        bool is_component_registered() const
        {
            return is_component_registered(typeid(T));
        }

        bool is_component_registered(std::type_index index) const
        {
            return m_component_table.contains(index);
        }

        Entity& create_entity(std::string_view name = "");

        template<class ...Args>
        Entity& create_entity(std::string_view name = "")
        {
            auto &entity = create_entity(name);

            (add_components<Args>(&entity), ...);

            return entity;
        }

        void add_to_group(std::string_view group_name, Entity& entity);

        void remove_from_group(std::string_view group_name, Entity& entity);

        void remove_group(std::string_view group_name);

        void create_group(std::string_view group_name);

        // if trim_invalid_entities is true it will search the group for any entities that have been destroyed and remove them from the group
        [[nodiscard]]
        std::vector<EntityViewHandle>* get_group(std::string_view group_name, bool trim_invalid_entities = true);

        [[nodiscard]]
        EntityViewHandle get_entity(std::string_view name);

        auto& get_all_groups()
        {
            return m_groups;
        }

        void destroy_children(Entity *entity);

        void destroy_entity(Entity *entity);

        template<class T>
        inline T* add_component(Entity *entity)
        {
            if (!is_component_registered<T>())
            {
                register_component<T>();
            }

            return (T*)add_component(entity, typeid(T));
        }

       u8* add_component(Entity *entity, std::type_index index);

        template<class ...Args>
        void add_components(Entity *entity)
        {
            (add_component<Args>(entity), ...);
        }

        template<class T>
        EcsResult remove_component(Entity *entity)
        {
            return remove_component(entity, typeid(T));
        }

        EcsResult remove_component(Entity *entity, std::type_index index);

        void update() override;

        std::vector<EntityEntry>& get_entities()
        {
            return m_entities_table[0].entities;
        }

        std::vector<EntitiesTableEntry>& get_all_entities()
        {
            return m_entities_table;
        }

        inline HashMap<std::type_index, ComponentType>& get_component_table()
        {
            return m_component_table;
        }

        [[nodiscard]]
        inline bool is_index_valid(u32 table, u32 slot) const
        {
            return table < m_entities_table.size() && slot < m_entities_table[table].entities.size();
        }

        void clear();

        Timer timer;

    private:
        friend Entity;
        friend EntityView;

        HashMap<std::type_index, ComponentType> m_component_table;
        HashMap<std::string, EntityViewHandle, ENABLE_TRANSPARENT_HASH> m_name_table;
        HashMap<std::string, std::vector<EntityViewHandle>, ENABLE_TRANSPARENT_HASH> m_groups;
        // holds on to all the entities that have a dirty flag set in their hierarchy. will be cleared once updated
        // the entity view is always the top most parent in that hierarchy. could potentially be optimized to only store
        // the part of the hierarchy that actually needs to be updated
        mutable std::mutex m_dirty_table_mutex;
        std::vector<EntityViewHandle> m_entity_dirty_table;
        std::vector<std::type_index> m_update_table;
        // stores an array of all entity arrays in the nexus including nested array of entities (child entities)
        std::vector<EntitiesTableEntry> m_entities_table;
        std::vector<std::pair<std::type_index, size_t>> m_remove_queue;
        EntityID m_id_counter {};
    };

    template<class T>
    T* Entity::add_component()
    {
        return m_nexus->add_component<T>(this);
    }

    template<class ... Args>
    void Entity::add_components()
    {
        m_nexus->add_components<Args...>(this);
    }

    template<class T>
    T* Entity::get_component()
    {
        auto iter = m_components.find(typeid(T));

        if (iter == m_components.end())
        {
            return nullptr;
        }

        return (T*)iter->second;
    }

    template<class ... Args>
    Entity& Entity::emplace_child(std::optional<std::string_view> name)
    {
        auto should_create_children = m_children_index == 0;

        auto &children =
            should_create_children
            ? m_nexus->m_entities_table.emplace_back(EntitiesTableEntry{get_view(), {}}).entities
            : m_nexus->m_entities_table[m_children_index].entities;

        auto index = children.size();
        auto &[entity, handle] = children.emplace_back();

        if (should_create_children)
        {
            m_children_index = m_nexus->m_entities_table.size() - 1;
        }

        entity.m_parent = get_view();
        entity.m_top_most_parent = get_top_most_parent();
        entity.m_nexus = m_nexus;
        entity.m_table_index = m_children_index;
        entity.m_index = index;
        entity.m_id = m_nexus->m_id_counter++;

        handle = entity.make_handle();

        if (name)
        {
            entity.m_name = name.value();
            m_nexus->m_name_table[entity.m_name] = entity.get_view();
        }

        (m_nexus->add_components<Args>(&entity), ...);

        return entity;
    }

    template<class T>
    EcsResult Entity::remove_component()
    {
        return m_nexus->remove_component<T>(this);
    }
}
