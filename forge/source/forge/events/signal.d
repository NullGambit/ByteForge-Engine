module forge.events.signal;

import forge.container.list;

struct Signal(R, A...)
{
    alias Delegate = R delegate(A);

    private
    {
        struct Connection
        {
            Delegate fn;
            uint previousFreeSlot = uint.max;
        }

        List!Connection m_connections;
        uint m_nextFreeSlot = uint.max;
    }

    uint connect(Delegate fn)
    {
        if (m_nextFreeSlot != uint.max)
        {
            auto freeConnection = &m_connections[m_nextFreeSlot];

            auto id = m_nextFreeSlot;

            m_nextFreeSlot = freeConnection.previousFreeSlot;

            freeConnection.previousFreeSlot = uint.max;

            return id;
        }

        m_connections.append(Connection(fn));

        return m_connections.length - 1;
    }

    void disconnect(uint id)
    {
        if (id >= m_connections.length)
        {
            return;
        }

        if (m_nextFreeSlot != uint.max)
        {
            m_connections[m_nextFreeSlot].previousFreeSlot = m_nextFreeSlot;
        }

        m_connections[id].fn = null;

        m_nextFreeSlot = id;
    }

    void opCall(A args)
    {
        import core.lifetime;

        foreach (ref connection; m_connections)
        {
            if (connection.fn != null)
            {
                connection.fn(forward!args);
            }
        }
    }
}
