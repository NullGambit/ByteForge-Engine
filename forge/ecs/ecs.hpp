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
#include "transform.hpp"
#include "forge/util/types.hpp"

// should get included as a part of ecs.hpp. don't remove this.
#include "macro_warcrimes.hpp"
#include "forge/container/array.hpp"
#include "forge/container/view.hpp"
#include "forge/container/virtual_array.hpp"
#include "forge/events/timer.hpp"

// the maximum amount of virtual memory that will be used for each component by default unless specified otherwise by the component
#define DEFAULT_ECS_MAX_MAPPED_MEMORY MB(48)
#define ECS_ENTITY_POOL_SIZE  500'000
#define ECS_CHILD_LIMIT 32

namespace forge
{
    class Nexus;
    class Entity;

    using EntityID = u32;

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
        bool m_is_valid = true;
        // if false this component should not be updated
        bool m_is_enabled = true;

        u32 m_id;

    protected:
        Entity *m_owner;

        virtual void on_enabled() {}
        virtual void on_disabled() {}
        virtual void on_create() {}
        virtual void on_destroy() {}

        // Convenience method to add a timer inside the nexus this component belongs to
        TimerID add_timer(TimerOptions &&options) const;

        void stop_timer(TimerID id) const;
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

        VirtualArray<Entity> get_children() const;

        [[nodiscard]]
        inline bool has_children() const
        {
            return m_children.get_length() > 0;
        }

        [[nodiscard]]
        size_t get_children_count() const;

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
            update_dirty_array();

            m_transform = transform;

            m_transform.m_parent = m_parent ? &m_parent->m_transform : nullptr;
        }

        [[nodiscard]]
        inline std::string_view get_name() const
        {
            return m_name;
        }

        void set_name(std::string_view new_name);

        [[nodiscard]]
        inline const HashMap<std::type_index, IComponent*>& get_components() const
        {
            return m_components;
        }

        template<class ...Args>
        Entity* emplace_child(std::string_view name = "");

        template<class T>
        void remove_component();

        void remove_component(std::type_index index);

        void on_editor_enter();

        void update_hierarchy();

        void destroy();

        [[nodiscard]]
        inline bool has_parent() const
        {
            return m_parent != nullptr;
        }

        [[nodiscard]]
        inline Entity* get_parent()
        {
            return m_parent;
        }

        [[nodiscard]]
        inline Entity* get_top_most_parent()
        {
            if (m_parent == nullptr)
            {
                return this;
            }

            return m_top_most_parent;
        }

        [[nodiscard]]
        inline Transform get_top_most_parent_transform()
        {
            return get_top_most_parent()->m_transform;
        }

        inline void set_local_position(glm::vec3 position)
        {
            update_dirty_array();
            m_transform.set_local_position(position);
        }

        inline glm::vec3 get_local_position() const
        {
            return m_transform.get_local_position();
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
            return m_transform.get_local_rotation();
        }

        inline glm::vec3 get_local_euler_rotation() const
        {
            return m_transform.get_local_euler_rotation();
        }

        inline void set_scale(glm::vec3 scale)
        {
            update_dirty_array();
            m_transform.set_scale(scale);
        }

        inline glm::vec3 get_scale() const
        {
            return m_transform.get_scale();
        }

        inline void set_position(glm::vec3 position)
        {
            update_dirty_array();
            m_transform.set_position(position);
        }

        inline glm::vec3 get_position()
        {
            return m_transform.get_position();
        }

        inline void set_rotation(glm::vec3 euler_rotation)
        {
            update_dirty_array();
            m_transform.set_rotation(euler_rotation);
        }

        inline void set_rotation(glm::quat rotation)
        {
            update_dirty_array();
            m_transform.set_rotation(rotation);
        }

        inline glm::quat get_rotation()
        {
            return m_transform.get_rotation();
        }

        inline glm::vec3 get_euler_rotation()
        {
            return m_transform.get_euler_rotation();
        }

        inline glm::mat4 get_model() const
        {
            return m_transform.get_global_matrix();
        }

        [[nodiscard]]
        inline Nexus* get_nexus()
        {
            return m_nexus;
        }

        [[nodiscard]]
        inline bool is_valid() const
        {
            return m_is_valid;
        }

        bool operator==(const Entity &other) const
        {
            return m_id == other.m_id;
        }

        bool operator==(const Entity *other) const
        {
            return m_id == other->m_id;
        }

    private:
        friend Nexus;

        Transform m_transform;

        std::string m_name;

        HashMap<std::type_index, IComponent*> m_components;

        Nexus *m_nexus;

        Entity *m_parent = nullptr;

        // the highest parent in this hierarchy
        // this is mostly for global transform calculations and for the entity dirty table
        Entity *m_top_most_parent = nullptr;

        VirtualArray<Entity> m_children;

        EntityID m_id;

        // will be set to true if any transform component is marked dirty and signifies that this entity has been marked for cleaning
        // is mainly used to avoid trying to add it to the dirty table more than once since the table is a vector
        bool m_is_queued_for_cleaning = false;

        // set to false if this entity is deallocated
        bool m_is_valid = false;

        void update_dirty_array();

        void set_parent(Entity *parent);
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
        bool register_component(bool override_should_update = false)
        {
            constexpr auto &type = typeid(T);

            if (m_component_table.contains(type))
            {
                return false;
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
                return false;
            }

            if (ComponentShouldEverUpdate<T> || override_should_update)
            {
                m_update_table.emplace_back(type);
            }

            return emplaced.second;
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

        Entity* create_entity(std::string_view name = "", Entity *parent = nullptr);

        template<class ...Args>
        Entity* create_entity(std::string_view name = "", Entity *parent = nullptr)
        {
            auto *entity = create_entity(name, parent);

            (add_components<Args>(entity), ...);

            return entity;
        }

        void add_to_group(std::string_view group_name, Entity* entity);

        void remove_from_group(std::string_view group_name, Entity* entity);

        void remove_group(std::string_view group_name);

        void create_group(std::string_view group_name);

        // if trim_invalid_entities is true it will search the group for any entities that have been destroyed and remove them from the group
        [[nodiscard]]
        std::vector<Entity*>* get_group(std::string_view group_name, bool trim_invalid_entities = true);

        [[nodiscard]]
        Entity* get_entity(std::string_view name);

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
        void remove_component(Entity *entity)
        {
            remove_component(entity, typeid(T));
        }

        void remove_component(Entity *entity, std::type_index index);

        void update() override;

        inline HashMap<std::type_index, ComponentType>& get_component_table()
        {
            return m_component_table;
        }

        VirtualArray<Entity> get_entities();

        [[nodiscard]]
        inline size_t get_entity_count() const
        {
            return m_entities.get_length();
        }

        void clear();

        Timer timer;

    private:
        friend Entity;

        HashMap<std::type_index, ComponentType> m_component_table;
        HashMap<std::string_view, Entity*> m_name_table;
        HashMap<std::string, std::vector<Entity*>, ENABLE_TRANSPARENT_HASH> m_groups;
        // holds on to all the entities that have a dirty flag set in their hierarchy. will be cleared once updated
        // the entity view is always the top most parent in that hierarchy. could potentially be optimized to only store
        // the part of the hierarchy that actually needs to be updated
        mutable std::mutex m_dirty_table_mutex;
        std::vector<Entity*> m_entity_dirty_table;
        std::vector<std::type_index> m_update_table;
        VirtualArray<Entity> m_entities;
        u64 m_id_counter {};
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
    Entity* Entity::emplace_child(std::string_view name)
    {
        return m_nexus->create_entity<Args...>(name, this);
    }

    template<class T>
    void Entity::remove_component()
    {
        return m_nexus->remove_component<T>(this);
    }
}
