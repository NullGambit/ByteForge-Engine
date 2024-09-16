#pragma once

#include <cstdint>
#include <typeindex>
#include <concepts>
#include <variant>

#include "../container/map.hpp"
#include "../fmt/fmt.hpp"
#include "../memory/defs.hpp"
#include "../memory/mem_pool.hpp"
#include "macro_warcrimes.hpp"
#include "../events/signal.hpp"
#include "core/isub_system.hpp"

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

    enum class EntityState
    {
        Invalid,
        Enabled,
        Disabled,
    };

    using DeltaTime = float;

    class Nexus;
    class Entity;

    struct ComponentView
    {
        size_t offset;
        uint8_t *pointer;
    };

    using OnComponentDestroy = forge::Signal<void()>;

    using FieldVar = std::variant<float*, double*, int*, std::string*>;

    struct ComponentField
    {
        std::string_view name;
        FieldVar var;
    };

    class BaseComponent
    {
    public:
        virtual ~BaseComponent() = default;

        virtual void update(DeltaTime delta) {}

        virtual std::vector<ComponentField> export_fields() { return {}; }

    private:
        friend Nexus;
        bool m_is_active = true;
        Entity *m_owner;
    };

    class Entity final
    {
    public:
        Entity() :
            m_state(EntityState::Enabled)
        {}

        inline EntityState get_state() const
        {
            return m_state;
        }

        template<class T>
        T* add_component(bool should_update = false);

        template<class ...Args>
        void add_components(bool should_update = false);

        template<class T>
        T* get_component(std::optional<void(*)()> on_destroy = std::nullopt);

        inline Entity *get_children()
        {
            return m_children;
        }

        inline Entity *get_sibling()
        {
            return m_siblings;
        }

        inline std::string_view get_name() const
        {
            return m_name;
        }

        void remove_child(Entity *child);

        void add_child(Entity *child);

        void foreach_child(std::function<void(Entity*)> callback, bool recursive = false, Entity *root = nullptr);

        template<class T>
        EcsResult remove_component();


    private:
        friend Nexus;

        std::string m_name;
        EntityState m_state;
        // byte offset inside the memory pool for freeing
        size_t m_offset;
        HashMap<std::type_index, ComponentView> m_components;

        Nexus  *m_nexus;
        Entity *m_parent = nullptr;
        Entity *m_children = nullptr;
        Entity *m_siblings = nullptr;
    };


    class Nexus final : public ISubSystem
    {
        struct ComponentType
        {
            MemPool mem_pool;
            HashMap<size_t, OnComponentDestroy> destroy_signals;

            template<class T>
            void free(size_t offset_to_free)
            {
                auto *mem = (T*)mem_pool.memory() + offset_to_free;

                if (std::derived_from<T, BaseComponent>)
                {
                    auto *component = (BaseComponent*)mem;
                    component->m_is_active = false;
                }

                mem->~T();

                mem_pool.free(offset_to_free);
            }

            void update(DeltaTime delta) const;
        };

    public:

        std::string init() override;

        void shutdown() override;

        bool is_critical() override { return true; }

        template<class T>
        EcsResult register_component(bool should_update = false)
        {
            const auto &type = typeid(T);

            if (m_component_table.contains(type))
            {
                return EcsResult::ComponentAlreadyRegistered;
            }

            auto emplaced = m_component_table.emplace(type, ComponentType{});

            auto result = emplaced.first->second.mem_pool.init(sizeof(T), ECS_MAX_MAPPED_MEMORY);

            if (!result)
            {
                return EcsResult::CouldNotAllocateComponentMemory;
            }

            if (std::derived_from<T, BaseComponent> && should_update)
            {

                m_update_table.emplace_back(type);
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

        inline bool is_entity_valid(Entity *entity) const
        {
            return entity == nullptr || entity->m_state != EntityState::Invalid;
        }

        template<class ...Args>
        Entity* create_entity(std::optional<std::string_view> name = std::nullopt)
        {
            auto [entity, _] = m_entities.emplace<Entity>();

            entity->m_nexus = this;

            // TODO: resolve name collisions
            if (name.has_value())
            {
                entity->m_name = name.value();
                m_name_table.emplace(entity->m_name, entity);
            }

            (add_components<Args>(entity), ...);

            return entity;
        }

        Entity* get_entity(std::string_view name);

        void delete_entity(Entity *entity);

        template<class T>
        T* add_component(Entity *entity, bool should_update = false)
        {
            if (!is_entity_valid(entity))
            {
                return nullptr;
            }

            if (!is_component_registered<T>())
            {
                register_component<T>(should_update);
            }

            auto &ct = m_component_table[typeid(T)];

            auto [ptr, offset] = ct.mem_pool.emplace<T>();

            if constexpr (std::derived_from<T, BaseComponent>)
            {
                ptr->m_owner = entity;
            }

            entity->m_components[typeid(T)] =
            {
                .offset =  offset,
                .pointer = (uint8_t*)ptr
            };

            return ptr;
        }

        template<class ...Args>
        void add_components(Entity *entity, bool should_update = false)
        {
            (add_component<Args>(entity, should_update), ...);
        }

        template<class T>
        EcsResult remove_component(Entity *entity)
        {
            if (!is_entity_valid(entity))
            {
                return EcsResult::EntityDoesNotExist;
            }

            auto iter = entity->m_components.find(typeid(T));

            if (iter == entity->m_components.end())
            {
                return EcsResult::EntityDoesNotHaveComponent;
            }

            auto &ct = m_component_table[typeid(T)];

            auto destroy_iter = ct.destroy_signals.find(iter->second.offset);

            if (destroy_iter != ct.destroy_signals.end())
            {
                auto &signal = destroy_iter->second;
                signal();
                ct.destroy_signals.erase(destroy_iter);
            }

            ct.free<T>(iter->second.offset);

            entity->m_components.erase(iter);

            return EcsResult::Ok;
        }

        void update() override
        {
            for (auto &type : m_update_table)
            {
                auto iter = m_component_table.find(type);

                if (iter == m_component_table.end())
                {
                    continue;
                }

                iter->second.update(5.32);
            }
        }

    private:
        friend Entity;

        HashMap<std::type_index, ComponentType> m_component_table;
        HashMap<std::string_view, Entity*> m_name_table;
        std::vector<std::type_index> m_update_table;
        MemPool m_entities;
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
