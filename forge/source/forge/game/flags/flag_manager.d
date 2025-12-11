module forge.game.flags.flag_manager;

import forge.container;
import core.atomic;

static struct FlagManager
{
    // private
    // {
    //     __gshared Map!(String, int) m_nameTable;
    //     shared static int m_handleCounter;
    // }

    static int getHandle(S)(auto ref S name)
    {
        // auto handle = m_nameTable.get(name);

        // if (handle == null)
        // {
        //     auto newHandle = atomicOp!"+="(m_handleCounter, 1);

        //     synchronized m_nameTable.put(name, newHandle);

        //     return newHandle;
        // }

        // return *handle;
        //
        return 0;
    }
}
