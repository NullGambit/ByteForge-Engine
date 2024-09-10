#pragma once
#include <functional>
#include <list>

namespace forge
{
    template<class T>
    concept IsFunction = requires(T)
    {
        std::is_function_v<T>;
    };

    template<class T>
    class Signal;

    template<class R, class ...A>
    class Connection
    {
    public:
        bool is_active = true;

        template<class Obj, IsFunction Fn>
        Connection(Obj *obj, Fn fn)
        {
            m_delegate = [obj, fn](A ...a)
            {
                return (obj->*fn)(a...);
            };
        }

        template<IsFunction Fn>
        explicit Connection(Fn fn)
        {
            m_delegate = [fn](A ...a)
            {
                return fn(a...);
            };
        }

        R operator()(A ...a) const
        {
            return m_delegate(a...);
        }

        bool is_alive() const
        {
            return m_alive;
        }

    private:
        friend Signal<R(A...)>;

        std::function<R(A...)> m_delegate;

        bool m_alive = true;
        typename std::list<Connection>::iterator m_iter;
    };

    template<class R, class ...A>
    class Signal<R(A...)>
    {
    public:
        using connection_t = Connection<R, A...>;

        template<class Obj, IsFunction Fn>
        connection_t* connect(Obj *obj, Fn fn)
        {
            auto &iter = m_connections.emplace_back(obj, fn);

            iter.m_iter = --m_connections.end();

            return &m_connections.back();
        }

        template<IsFunction Fn>
        connection_t* connect(Fn fn)
        {
            auto &iter = m_connections.emplace_back(fn);

            iter.m_iter = --m_connections.end();

            return &m_connections.back();
        }

        void disconnect(connection_t *connection)
        {
            if (!connection->m_alive)
            {
                return;
            }

            connection->m_alive = false;

            m_connections.erase(connection->m_iter);
        }

        void operator()(A ...a) const
        {
            for (const auto &connection : m_connections)
            {
                if (connection.is_active)
                {
                    connection(a...);
                }
            }
        }

        std::vector<R> call_with_values(A ...a) const
        {
            std::vector<R> values;

            values.reserve(m_connections.size());

            for (const auto &connection : m_connections)
            {
                if (connection.is_active)
                {
                    values.emplace_back(connection(a...));
                }
            }

            return values;
        }

    private:
        std::list<Connection<R, A...>> m_connections;
    };
}
