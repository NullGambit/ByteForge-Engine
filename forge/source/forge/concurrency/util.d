module forge.concurrency.util;

import core.thread;
import std.concurrency;

void setThreadName(string name)
{
    Thread.getThis().name = name;
}

string getThreadName()
{
    if (Thread.getThis().name != "")
    {
        return Thread.getThis().name;
    }

    import std.format;

    return format("%s", thisTid);
}
