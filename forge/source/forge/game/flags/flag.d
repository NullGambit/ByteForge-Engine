module forge.game.flags.flag;

import forge.container.string_view;
import forge.game.flags.flag_manager;

struct Flag
{
    int handle;

    this(int handle)
    {
        this.handle = handle;
    }

    this(S)(auto ref S name)
    {
        handle = FlagManager.getHandle(name);
    }
}
