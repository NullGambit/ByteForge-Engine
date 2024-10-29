#pragma once

#include <functional>

namespace forge
{
    template<class T>
    concept IsFunction = requires(T)
    {
        std::is_function_v<T>;
    };

    template<class T>
    class Signal;

    using ConnectionID = u32;

    template<class R, class ...A>
    class ScopedConnection
    {
    public:

        ScopedConnection() = default;

        ScopedConnection(ConnectionID id, Signal<R(A...)> *signal) :
            m_id(id),
            m_signal(signal)
        {}

        ~ScopedConnection();

    private:
        ConnectionID m_id;
        Signal<R(A...)> *m_signal = nullptr;
    };

    template<class R, class ...A>
    class Delegate
    {
    public:
        bool is_active = true;

        template<class Obj, IsFunction Fn>
        Delegate(Obj *obj, Fn fn)
        {
            m_delegate = [obj, fn](A ...a)
            {
                return (obj->*fn)(a...);
            };
        }

        template<IsFunction Fn>
        explicit Delegate(Fn fn)
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
            return m_is_alive;
        }

    private:
        friend Signal<R(A...)>;

        std::function<R(A...)> m_delegate;

        bool m_is_alive = true;
    };

    template<class R, class ...A>
    class Signal<R(A...)>
    {
    public:
        using Delegate = Delegate<R, A...>;

    private:
        // value used to represent that there is no free slot index here
        static constexpr u32 NO_FREE_SLOTS = std::numeric_limits<u32>::max();

        struct Connection
        {
            Delegate delegate;
            // previous free slot in connection list
            u32 previous_free_slot = NO_FREE_SLOTS;
        };

    public:

        template<class Obj, IsFunction Fn>
        ConnectionID connect(Obj *obj, Fn fn)
        {
            return connect_implementation(Delegate{ obj, fn });
        }

        template<IsFunction Fn>
        ConnectionID connect(Fn fn)
        {
            return connect_implementation(Delegate{ fn });
        }

        template<class Obj, IsFunction Fn>
        ScopedConnection<R, A...> scoped_connect(Obj *obj, Fn fn)
        {
            return {connect_implementation(Delegate{ obj, fn }), this};
        }

        template<IsFunction Fn>
        ScopedConnection<R, A...> scoped_connect(Fn fn)
        {
            return {connect_implementation(Delegate{ fn }), this};
        }

        void disconnect(ConnectionID id)
        {
            auto &[delegate, _] = m_connections[id];

            delegate.m_is_alive = false;

            if (m_free_slot != NO_FREE_SLOTS)
            {
                m_connections[m_free_slot].previous_free_slot = m_free_slot;
            }

            m_free_slot = id;
        }

        void operator()(A ...a) const
        {
            for (const auto &[delegate, _] : m_connections)
            {
                if (delegate.is_active && delegate.m_is_alive)
                {
                    delegate(a...);
                }
            }
        }

        std::vector<R> call_with_return_values(A ...a) const
        {
            std::vector<R> values;

            values.reserve(m_connections.size());

            for (const auto &[delegate, _] : m_connections)
            {
                if (delegate.is_active && delegate.m_is_alive)
                {
                    values.emplace_back(delegate(a...));
                }
            }

            return values;
        }

    private:
        std::vector<Connection> m_connections;
        u32 m_free_slot = NO_FREE_SLOTS;

        ConnectionID connect_implementation(Delegate &&delegate)
        {
            // get index from a free slot if it exists
            if (m_free_slot != NO_FREE_SLOTS)
            {
                auto &[free_delegate, previous_free_slot] = m_connections[m_free_slot];

                free_delegate = std::move(delegate);

                auto index = m_free_slot;

                m_free_slot = previous_free_slot;

                return index;
            }

            auto index = m_connections.size();

            auto &iter = m_connections.emplace_back(std::forward<Delegate>(delegate));

            return index;
        }
    };

    template<class R, class ... A>
    ScopedConnection<R, A...>::~ScopedConnection()
    {
        if (m_signal)
        {
            m_signal->disconnect(m_id);
        }
    }
}
