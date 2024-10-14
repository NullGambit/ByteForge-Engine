#pragma once

#include <cstdint>
#include <typeindex>
#include <concepts>
#include <variant>

#include "../container/map.hpp"
#include "../fmt/fmt.hpp"
#include "../memory/defs.hpp"
#include "../memory/mem_pool.hpp"
#include "../events/signal.hpp"
#include "core/isub_system.hpp"

// should get included as a part of ecs.hpp. don't remove this.
#include "macro_warcrimes.hpp"
#include "transform.hpp"
#include "util/types.hpp"

#define ECS_MAX_MAPPED_MEMORY GB(8)

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

    using DeltaTime = float;

    class Nexus;
    class Entity;

    struct ComponentView
    {
        size_t offset;
        uint8_t *pointer;
    };

    struct EntityView
    {
        static constexpr auto NO_INDEX = std::numeric_limits<u32>::max();

        Nexus *nexus;
        u32 index = NO_INDEX;
        u32 table;

        Entity& get();

        inline bool has_value() const
        {
            return index != NO_INDEX;
        }
    };

    using OnComponentDestroy = Signal<void()>;

    using FieldVar = std::variant<float*, double*, int*, std::string*>;

    struct ComponentField
    {
        std::string_view name;
        FieldVar var;
    };

    class IComponent
    {
    public:
        virtual ~IComponent() = default;

        virtual void update(DeltaTime delta) {}

        void set_enabled(bool value);

        inline bool is_enabled() const
        {
            return m_is_enabled;
        }

        virtual std::vector<ComponentField> export_fields() { return {}; }

        virtual void on_editor_enter() {}

    private:
        friend Nexus;
        // if false this component has been freed
        bool m_is_active = true;
        // if false this component should not be updated
        bool m_is_enabled = true;

    protected:
        EntityView m_owner;

        virtual void on_enabled() {}
        virtual void on_disabled() {}
    };

    class Entity final
    {
    public:

        template<class T>
        T* add_component(bool should_update = false);

        u8* add_component(std::type_index index);

        template<class ...Args>
        void add_components(bool should_update = false);

        template<class T>
        T* get_component(std::optional<void(*)()> on_destroy = std::nullopt);

        std::vector<Entity>& get_children();

        [[nodiscard]]
        inline u32 get_children_index() const
        {
            return m_children_index;
        }

        inline std::string_view get_name() const
        {
            return m_name;
        }

        void set_name(std::string_view new_name);

        inline const HashMap<std::type_index, ComponentView>& get_components() const
        {
            return m_components;
        }

        Entity& emplace_child(std::optional<std::string_view> name = std::nullopt);

        template<class T>
        EcsResult remove_component();

        EcsResult remove_component(std::type_index index);

        void on_editor_enter();

        void update_hierarchy();

        inline EntityView get_view()
        {
            return {m_nexus, m_index, get_entity_table()};
        }

        void destroy();

    private:
        friend Nexus;
        friend EntityView;

        Transform m_transform;

        std::string m_name;
        HashMap<std::type_index, ComponentView> m_components;

        Nexus  *m_nexus = nullptr;
        EntityView m_parent {};
        u32 m_children_index = 0;
        u32 m_index;

        // gets the table this entity belongs to (see m_index for more info)
        u32 get_entity_table();
    };

    class Nexus final : public ISubSystem
    {
        struct ComponentType
        {
            MemPool mem_pool;
            HashMap<size_t, OnComponentDestroy> destroy_signals;
            bool is_component = false;

            void free(size_t offset_to_free)
            {
                auto *mem = mem_pool.get_memory() + offset_to_free;

                if (is_component)
                {
                    auto *component = (IComponent*)mem;
                    component->m_is_active = false;
                }

                mem_pool.free(offset_to_free);
            }

            void update(DeltaTime delta) const;
        };

    public:

        std::string init() override;

        void shutdown() override;

        bool is_critical() override { return true; }

        // SubSystemThreadMode get_thread_mode() override
        // {
        //     return SubSystemThreadMode::OffloadThread;
        // }

        template<class T>
        EcsResult register_component(bool should_update = false)
        {
            const auto &type = typeid(T);

            if (m_component_table.contains(type))
            {
                return EcsResult::ComponentAlreadyRegistered;
            }

            auto emplaced = m_component_table.emplace(type, ComponentType{});

            auto &ct = emplaced.first->second;

            auto result = ct.mem_pool.init<T>(ECS_MAX_MAPPED_MEMORY);

            if (!result)
            {
                return EcsResult::CouldNotAllocateComponentMemory;
            }

            if (std::derived_from<T, IComponent>)
            {
                ct.is_component = true;

                if (should_update)
                {
                    m_update_table.emplace_back(type);
                }
            }

            return emplaced.second ? EcsResult::Ok : EcsResult::CouldNotAddComponentToTypeMap;
        }

        // TODO: remove from update table as well
        template<class T>
        void unregister_component()
        {
            auto it = m_component_table.find(typeid(T));

            if (it == m_component_table.end())
            {
                return;
            }

            it->second.mem_pool.destroy();

            m_component_table.erase(it);
        }

        template<class T>
        bool is_component_registered() const
        {
            return m_component_table.contains(typeid(T));
        }

        bool is_component_registered(std::type_index index) const
        {
            return m_component_table.contains(index);
        }

        template<class ...Args>
        Entity* create_entity(std::optional<std::string_view> name = std::nullopt)
        {
            auto &entities = m_entities.front();
            auto index = entities.size();
            auto &entity = entities.emplace_back();

            entity.m_nexus = this;
            entity.m_index = index;

            // TODO: resolve name collisions
            if (name)
            {
                entity.m_name = name.value();
                m_name_table.emplace(entity.m_name, entity.get_view());
            }

            (add_components<Args>(&entity), ...);

            return &entity;
        }

        Entity* get_entity(std::string_view name);

        void destroy_entity(Entity *entity);

        template<class T>
        inline T* add_component(Entity *entity, bool should_update = false)
        {
            if (!is_component_registered<T>())
            {
                register_component<T>(should_update);
            }

            return (T*)add_component(entity, typeid(T));
        }

       u8* add_component(Entity *entity, std::type_index index);

        template<class ...Args>
        void add_components(Entity *entity, bool should_update = false)
        {
            (add_component<Args>(entity, should_update), ...);
        }

        template<class T>
        EcsResult remove_component(Entity *entity)
        {
            return remove_component(entity, typeid(T));
        }

        EcsResult remove_component(Entity *entity, std::type_index index)
        {
            auto iter = entity->m_components.find(index);

            if (iter == entity->m_components.end())
            {
                return EcsResult::EntityDoesNotHaveComponent;
            }

            auto &ct = m_component_table[index];

            ct.free(iter->second.offset);

            entity->m_components.erase(iter);

            return EcsResult::Ok;
        }

        void update() override;

        std::vector<Entity>& get_entities()
        {
            return m_entities[0];
        }

        std::vector<std::vector<Entity>>& get_all_entities()
        {
            return m_entities;
        }

        inline HashMap<std::type_index, ComponentType>& get_component_table()
        {
            return m_component_table;
        }

    private:
        friend Entity;
        friend EntityView;

        HashMap<std::type_index, ComponentType> m_component_table;
        HashMap<std::string, EntityView, ENABLE_TRANSPARENT_HASH> m_name_table;
        std::vector<std::type_index> m_update_table;
        std::vector<std::vector<Entity>> m_entities;
        std::vector<std::pair<std::type_index, size_t>> m_remove_queue;
    };

    template<class T>
    T* Entity::add_component(bool should_update)
    {
        return m_nexus->add_component<T>(this, should_update);
    }

    template<class ... Args>
    void Entity::add_components(bool should_update)
    {
        m_nexus->add_components<Args...>(this, should_update);
    }

    template<class T>
    T* Entity::get_component(std::optional<void(*)()> on_destroy)
    {
        auto iter = m_components.find(typeid(T));

        if (iter == m_components.end())
        {
            return nullptr;
        }

        if (on_destroy.has_value())
        {
            auto ct_iter = m_nexus->m_component_table.find(typeid(T));
            auto offset = iter->second.offset;

            if (ct_iter != m_nexus->m_component_table.end())
            {
                auto &ct = ct_iter->second;

                auto destroy_iter = ct.destroy_signals.find(offset);
                auto &signal = destroy_iter->second;

                if (destroy_iter == ct.destroy_signals.end())
                {
                    auto emplaced = ct.destroy_signals.emplace(offset, OnComponentDestroy());
                    signal = emplaced.first->second;
                }

                signal.connect(on_destroy.value());
            }
        }

        return (T*)iter->second.pointer;
    }

    template<class T>
    EcsResult Entity::remove_component()
    {
        return m_nexus->remove_component<T>(this);
    }
}
