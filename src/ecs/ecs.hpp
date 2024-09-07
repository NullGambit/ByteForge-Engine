#pragma once

#include <cstdint>
#include <typeindex>
#include <concepts>

#include "../container/map.hpp"
#include "../fmt/fmt.hpp"
#include "../memory/defs.hpp"
#include "../memory/mem_pool.hpp"

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

    class BaseComponent
    {
    public:
        virtual ~BaseComponent() = default;

        virtual void update(DeltaTime delta) {}
    private:
        friend Nexus;
        bool m_is_active = true;
        Entity *m_owner;
        Nexus *m_nexus;
    };

    class Entity final
    {
    public:
        Entity() :
            m_state(EntityState::Enabled)
        {}

    private:
        friend Nexus;

        EntityState m_state;
        // byte offset inside the memory pool for freeing
        size_t m_offset;
        forge::HashMap<std::type_index, ComponentView> m_components;
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

        Entity* create_entity()
        {
            auto [entity, _] = m_entities.allocate<Entity>();
            return entity;
        }

        template<class ...Args>
        Entity* create_entity()
        {
            auto [entity, _] = m_entities.allocate<Entity>();

            ((add_components<Args>(entity)), ...);

            return entity;
        }

        void delete_entity(Entity *entity)
        {
            if (!is_entity_valid(entity))
            {
                return;
            }

            entity->m_state = EntityState::Invalid;
            entity->m_components.clear();

            m_entities.free(entity->m_offset);
        }

        template<class T>
        T* add_component(Entity *entity, bool should_update = false, std::optional<std::string_view> name = std::nullopt)
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

            entity->m_components[typeid(T)] =
            {
                .offset =  offset,
                .pointer = (uint8_t*)ptr
            };

            if (name.has_value())
            {
                m_name_table.emplace(name.value(), entity);
            }

            return ptr;
        }

        template<class ...Args>
        void add_components(Entity *entity)
        {
            ((add_component<Args>(entity), ...));
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

        template<class T>
        T* get_component(Entity *entity)
        {
            if (!is_entity_valid(entity))
            {
                return nullptr;
            }

            return (T*)entity->m_components[typeid(T)].pointer;
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
        forge::HashMap<std::string, Entity*, ENABLE_TRANSPARENT_HASH> m_name_table;
        forge::MemPool m_entities;
        std::vector<std::type_index> m_update_table;
    };
}
