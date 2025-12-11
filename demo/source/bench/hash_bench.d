module bench.hash_bench;

import bench.util;
import std.datetime.stopwatch;
import forge.fmt;

void runHashBench()
{
    enum TotalKeys = 1_000_000;
    enum RunCount = 1;

    auto keys = makeKeys(TotalKeys, 124, 512);

    auto benchXxHash = ()
    {
        import forge.digest;
        ulong result;

        foreach (ref key; keys)
        {
            result += xxhash64(key.ptr, key.length);
        }
    };

    auto builtinHash = ()
    {
        ulong result;

        foreach (ref key; keys)
        {
            result += key.toHash();
        }
    };

    auto xxHashResult = benchmark!benchXxHash(RunCount);
    auto builtinResult = benchmark!builtinHash(RunCount);

    println("xxhash: {}", xxHashResult[0]);
    println("builtin: {}", builtinResult[0]);
}
