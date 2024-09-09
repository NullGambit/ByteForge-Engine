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

#define ECS_MAX_MAPPED_MEMORY GB(8)

namespace ecs
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
        T* get_component()
        {
            auto iter = m_components.find(typeid(T));

            if (iter == m_components.end())
            {
                return nullptr;
            }

            return (T*)iter->second.pointer;
        }

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

    private:
        friend Nexus;

        std::string m_name;
        EntityState m_state;
        // byte offset inside the memory pool for freeing
        size_t m_offset;
        forge::HashMap<std::type_index, ComponentView> m_components;

        Nexus  *m_nexus;
        Entity *m_parent = nullptr;
        Entity *m_children = nullptr;
        Entity *m_siblings = nullptr;
    };

    class Nexus final
    {
        struct ComponentType
        {
            forge::MemPool mem_pool;

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

        Nexus()
        {
            m_entities.init(sizeof(Entity), ECS_MAX_MAPPED_MEMORY);
        }

        template<class T>
        EcsResult register_component(bool should_update = false)
        {
            const auto &type = typeid(T);

            if (m_type_table.contains(type))
            {
                return EcsResult::ComponentAlreadyRegistered;
            }

            auto emplaced = m_type_table.emplace(type, ComponentType{});

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
            auto it = m_type_table.find(typeid(T));

            if (it == m_type_table.end())
            {
                return;
            }

            it->second.mem_pool.destroy();

            m_type_table.erase(it);
        }

        template<class T>
        bool is_component_registered() const
        {
            return m_type_table.contains(typeid(T));
        }

        inline bool is_entity_valid(Entity *entity) const
        {
            return entity == nullptr || entity->m_state != EntityState::Invalid;
        }

        template<class ...Args>
        Entity* create_entity(std::optional<std::string_view> name = std::nullopt)
        {
            auto [entity, _] = m_entities.allocate<Entity>();

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

            auto &ct = m_type_table[typeid(T)];

            auto [ptr, offset] = ct.mem_pool.allocate<T>();

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

            auto &ct = m_type_table[typeid(T)];

            ct.free<T>(iter->second.offset);

            entity->m_components.erase(iter);

            return EcsResult::Ok;
        }

        void update() const
        {
            for (auto &type : m_update_table)
            {
                auto iter = m_type_table.find(type);

                if (iter == m_type_table.end())
                {
                    continue;
                }

                iter->second.update(5.32);
            }
        }

    private:
        forge::HashMap<std::type_index, ComponentType> m_type_table;
        forge::HashMap<std::string_view, Entity*> m_name_table;
        std::vector<std::type_index> m_update_table;
        forge::MemPool m_entities;
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
}
