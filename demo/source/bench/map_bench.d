module bench.map_bench;

import forge.container;
import forge.fmt;

import bench.util;

void mapBench()
{
    enum EntryCount = 1_000_000;
    enum MinKeySize = 4;
    enum MaxKeySize = 32;
    enum RunCount = 1;

    auto mapEntries = makeKeys(EntryCount, MinKeySize, MaxKeySize);

    import std.datetime.stopwatch;
    import core.lifetime;

    void runForgeBench(alias Bucket, string name)()
    {
        Map!(String, int, Bucket) map;
        // map.rehash(EntryCount);

        auto putEntries = mapEntries.clone();

        auto benchPut = ()
        {
            foreach (i, ref entry; putEntries)
            {
                map.put(entry, cast(int)i);
            }
        };

        auto benchGet = ()
        {
            auto count = 0;

            foreach (ref entry; mapEntries)
            {
                auto value = map.get(entry);
                if (value)
                {
                    count += *value;
                }
            }
        };

        auto benchIter = ()
        {
            auto count = 0;
            foreach (ref key, ref value; map)
            {
                count += value;
            }
        };

        auto benchDel = ()
        {
            foreach (i, ref entry; mapEntries)
            {
                map.remove(entry);
            }
        };

        auto putResult = benchmark!benchPut(RunCount);
        auto getResult = benchmark!benchGet(RunCount);
        auto iterResult = benchmark!benchIter(RunCount);
        auto delResult = benchmark!benchDel(RunCount);

        println("{}::put: {}", name, putResult[0]);
        println("{}::get: {}", name, getResult[0]);
        println("{}::iter: {}", name, iterResult[0]);
        println("{}::del: {}", name, delResult[0]);
    }

    void runBuiltinBench(string name)()
    {
        int[String] map;

        auto putEntries = mapEntries.clone();

        auto benchPut = ()
        {
            foreach (i, ref entry; putEntries)
            {
                map[entry] = cast(int)i;
            }
        };

        auto benchGet = ()
        {
            auto count = 0;

            foreach (ref entry; mapEntries)
            {
                auto value = map[entry];
                if (value)
                {
                    count += value;
                }
            }
        };

        auto benchIter = ()
        {
            auto count = 0;
            foreach (ref key, ref value; map)
            {
                count += value;
            }
        };

        auto benchDel = ()
        {
            foreach (i, ref entry; mapEntries)
            {
                map.remove(entry);
            }
        };

        auto putResult = benchmark!benchPut(RunCount);
        auto getResult = benchmark!benchGet(RunCount);
        auto iterResult = benchmark!benchIter(RunCount);
        auto delResult = benchmark!benchDel(RunCount);

        println("{}::put: {}", name, putResult[0]);
        println("{}::get: {}", name, getResult[0]);
        println("{}::iter: {}", name, iterResult[0]);
        println("{}::del: {}", name, delResult[0]);
    }

    runForgeBench!(LinearProbeBucket, "forge_linear")();
    runForgeBench!(RobbinHoodProbing, "forge_robinhood")();
    runForgeBench!(SwissTableProbbing, "forge_swiss")();

    runBuiltinBench!("builtin")();
}
