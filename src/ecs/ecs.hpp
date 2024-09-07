#pragma once

#include <cstdint>
#include <typeindex>
#include <concepts>

#include "../container/map.hpp"
#include "../fmt/fmt.hpp"
#include "../memory/defs.hpp"

#define ECS_MAX_MAPPED_MEMORY GB(4)

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
    using EntityID = uint32_t;

    class Nexus;

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
    };

    class BaseEntity
    {
    public:
        explicit BaseEntity(EntityID id) :
            m_id(id),
            m_state(EntityState::Enabled)
        {}

    private:
        friend Nexus;

        EntityID m_id;
        EntityState m_state;
        forge::HashMap<std::type_index, ComponentView> m_components;
    };

    class Nexus
    {
        struct ComponentType
        {
            uint8_t *memory;
            size_t offset;
            size_t length;
            size_t component_size;
            std::vector<size_t> free_list;

            EcsResult init(size_t size);

            void destroy();

             std::pair<uint8_t*, size_t> allocate();

            template<class T>
            std::pair<T*, size_t> allocate()
            {
                auto [mem, offset] = allocate();

                return {new (mem) T(), offset};
            }

            void free(size_t offset_to_free);

            template<class T>
            void free(size_t offset_to_free)
            {
                auto *mem = (T*)memory + offset_to_free;

                if (std::derived_from<T, BaseComponent>)
                {
                    auto *component = (BaseComponent*)mem;
                    component->m_is_active = false;
                }

                mem->~T();
            }

            void update(DeltaTime delta) const;
        };

    public:

        template<class T>
        EcsResult register_component(bool should_update = false)
        {
            const auto &type = typeid(T);

            if (m_type_table.contains(type))
            {
                return EcsResult::ComponentAlreadyRegistered;
            }

            ComponentType ct;

            auto result = ct.init(sizeof(T));

            if (result != EcsResult::Ok)
            {
                return result;
            }

            auto emplaced = m_type_table.emplace(type, std::move(ct));

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

            it->second.destroy();

            m_type_table.erase(it);
        }

        template<class T>
        bool is_component_registered() const
        {
            return m_type_table.contains(typeid(T));
        }

        bool is_entity_valid(EntityID id) const
        {
            if (id >= m_entities.size())
            {
                return false;
            }

            return m_entities[id].m_state != EntityState::Invalid;
        }

        EntityID create_entity()
        {
            if (!m_entity_free_list.empty())
            {
                auto id = m_entity_free_list.back();

                m_entity_free_list.pop_back();

                auto &entity = m_entities[id];

                entity.m_state = EntityState::Enabled;

                return id;
            }

            auto id = m_entities.size();

            auto &entity = m_entities.emplace_back(id);

            entity.m_id = id;

            return id;
        }

        void delete_entity(EntityID id)
        {
            if (!is_entity_valid(id))
            {
                return;
            }

            auto &entity = m_entities[id];

            entity.m_state = EntityState::Invalid;
            entity.m_components.clear();

            m_entity_free_list.push_back(id);
        }

        template<class T>
        T* add_component(EntityID id, bool should_update = false, std::optional<std::string_view> name = std::nullopt)
        {
            if (!is_entity_valid(id))
            {
                return nullptr;
            }

            if (!is_component_registered<T>())
            {
                register_component<T>(should_update);
            }

            auto &entity = m_entities[id];

            auto &ct = m_type_table[typeid(T)];

            auto [ptr, offset] = ct.allocate<T>();

            entity.m_components[typeid(T)] =
            {
                .offset =  offset,
                .pointer = (uint8_t*)ptr
            };

            if (name.has_value())
            {
                m_name_table.emplace(name, entity.m_id);
            }

            return ptr;
        }

        template<class T>
        EcsResult remove_component(EntityID id)
        {
            if (!is_entity_valid(id))
            {
                return EcsResult::EntityDoesNotExist;
            }

            auto &entity = m_entities[id];

            auto iter = entity.m_components.find(typeid(T));

            if (iter == entity.m_components.end())
            {
                return EcsResult::EntityDoesNotHaveComponent;
            }

            auto &ct = m_type_table[typeid(T)];

            ct.free<T>(iter->second.offset);

            entity.m_components.erase(iter);

            return EcsResult::Ok;
        }

        template<class T>
        T* get_component(EntityID id)
        {
            if (!is_entity_valid(id))
            {
                return nullptr;
            }

            auto &entity = m_entities[id];

            return (T*)entity.m_components[typeid(T)].pointer;
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
        forge::HashMap<std::string, EntityID, ENABLE_TRANSPARENT_HASH> m_name_table;
        std::vector<BaseEntity> m_entities;
        std::vector<EntityID> m_entity_free_list;
        std::vector<std::type_index> m_update_table;
    };
}
