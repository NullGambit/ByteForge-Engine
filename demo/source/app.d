import forge.mem.mempool;
import std.stdio;
import std.concurrency;
import core.atomic;
import forge.mem.allocators.fmalloc;
import forge.mem.allocators;
import forge.mem.defs;
import forge.concurrency.util;
import std.typecons;
import core.sync.mutex;
import core.sync.condition;

__gshared uint tasks = 0;

class Data
{
    int x;
    int z;
    char[1024] padding;

    alias x this;

    this(int x = 0)
    {
        this.x = x;
    }
}

void threadedAllocTest()
{
    import std.conv : to;

    __gshared int count;

    count++;

    setThreadName("Worker_" ~ to!string(count));

    enum ObjCount = 60;

    // newObj!int();
    // newObj!int();
    // newObj!int();

    foreach (i; 0 .. ObjCount)
    {
        auto n = newObj!Data();

        scope (exit)
        {
            if (i % 2 != 0)
            {
                delObj(n);
            }

            // delObj(n);
        }

        n += i;

        // writeln(n);
    }

    synchronized tasks--;
}

void benchSimd()
{
    import forge.math.simd.vector : dot;
    import inteli;

    float4 a = [5, 1, 2];
    float4 b = [5, 1, 2];

    dot(a, b);
}

void benchScalar()
{

}

void main()
{
    import forge.math.vector;

    auto a = Vec3(1);
    auto b = Vec3(5, 1, 2);


    writeln(b.length());
    // setThreadName("Main");

    // setBlockSize(2.gbs);

    // import core.thread;

    // enum TaskCount = 16;

    // tasks = TaskCount;

    // foreach (_; 0 .. TaskCount)
    // {
    //     spawn(&threadedAllocTest);
    // }

    // while (tasks > 0)
    // {
    // }

    // stdout.flush();

    // import forge.mem.allocators.statistics;

    // auto stats = getStatistics();

    // foreach (i, const ref field; stats.tupleof[0 .. $ - 1])
    // {
    //     enum fieldName = AllocatorStatistics.tupleof[i].stringof;

    //     writefln("%s: %s", fieldName, field);
    // }

    // foreach (f; stats.footprint)
    // {
    //     auto i = 0;

    //     foreach (line; f.trace)
    //     {
    //         if (i++ > 7)
    //         {
    //             writeln(line);
    //             break;
    //         }
    //     }
    // }
}
