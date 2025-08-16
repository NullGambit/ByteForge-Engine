// a costum allocator designed for the needs and usage patterns of this game engine

module forge.mem.allocators.fmalloc;

import forge.mem.virtual_mem;
import forge.mem.defs;
import forge.mem.utils;
import std.concurrency;
import forge.mem.allocators.statistics;

@nogc:

enum DefaultBlockSize = 32.mbs;
enum DefaultMemorySize = 4.gbs;
enum MaxMemoryBlocks = 32;

private
{
    __gshared byte* g_memory;
    __gshared size_t g_size;
    __gshared size_t g_blockOffset;
    // any array of size MaxMemoryBlocks can use their index gotten from this counter
    // in order to access shared memory without needing to lock
    __gshared size_t g_blockCounter;
    __gshared AllocatorStatistics[MaxMemoryBlocks] g_statisticsTable;

    static MemoryBlock g_threadBlock;
    static size_t g_threadBlockSize = DefaultBlockSize;

    struct SizeClassContainer
    {
        uint maxSize;
        uint offset;

        uint* list;
    }

    enum MinSC = 32;
    enum MaxSC = 4096;

    enum SizeClasses
    {
        SCTiny,
        SC32,
        SC64,
        SC128,
        SC256,
        SC512,
        SC1024,
        SC2048,
        SC4096,
        SCHuge,
    }

    enum MaxSizeClasses = SizeClasses.max + 1;

    struct MemoryBlock
    {
        byte* memory;
        size_t globalOffset;
        size_t blockSize;
        size_t blockOffset;
        // the index of this memory block which is recieved from incrementing g_blockCounter
        size_t index;
        SizeClassContainer[MaxSizeClasses] sizeClasses;
    }

    struct MemoryHeader
    {
        // the offset in range of 0 .. blockOffset. a uint should be big enough for all addressable memory of a single memory block
        uint offset;
        uint size;
    }
}

// gets the combined statistics of all thread blocks. blocks all other threads
AllocatorStatistics getStatistics()
{
    AllocatorStatistics stats;

    synchronized
    {
        stats.blocks = g_blockCounter;
        debug (RecordAllocations)
        {
            foreach (key, value; g_statisticsTable[g_threadBlock.index].footprint)
            {
                stats.footprint[key] = value;
            }
        }

        foreach (statTable; g_statisticsTable)
        {
            stats.inUse += statTable.inUse;
            stats.free += statTable.free;
            stats.reserved += statTable.reserved;
        }
    }

    return stats;
}

void setBlockSize(size_t size)
{
    g_threadBlockSize = size;
}

static this()
{
    import forge.mem.virtual_mem;

    enum listSize = 1024;

    foreach (ref sc; g_threadBlock.sizeClasses)
    {
        sc.maxSize = listSize;
        sc.list = cast(uint*) virtualAlloc(listSize * uint.sizeof);
    }
}

static ~this()
{
    import forge.mem.virtual_mem;

    g_threadBlock.memory = null;

    foreach (ref sc; g_threadBlock.sizeClasses)
    {
        sc.maxSize = 0;
        sc.offset = 0;
        virtualFree(cast(byte*) sc.list);
    }
}

bool initFmalloc(size_t size)
{
    g_size = size;
    // TODO: make virtualAlloc be more configurable and use huge pages for large sizes automatically
    g_memory = virtualAlloc(size);
    return g_memory != null;
}

void finalizeFmalloc()
{
    virtualFree(g_memory);
    g_size = 0;
}

void initThreadBlock()
{
    g_threadBlock.blockSize = g_threadBlockSize;

    synchronized
    {
        if (g_memory == null)
        {
            auto ok = initFmalloc(DefaultMemorySize);
            assert(ok, "automatic init failed");
        }

        g_threadBlock.memory = g_memory + g_blockOffset;
        g_threadBlock.globalOffset = g_blockOffset;
        g_blockOffset += g_threadBlockSize;

        g_threadBlock.index = g_blockCounter++;

        g_statisticsTable[g_threadBlock.index].reserved += g_threadBlockSize;

        debug (LogAllocatorBlockInits)
        {
            import std.logger;
            import std.format;
            import forge.concurrency.util;

            info(format("initialized thread block %s of size %s", getThreadName(), g_threadBlockSize));
        }
    }
}

byte* fmalloc(size_t size, size_t alignment)
{
    import core.builtins : unlikely;

    if (unlikely(g_threadBlock.memory == null))
    {
        initThreadBlock();
    }

    const freeOffset = findFree(size);

    if (freeOffset != size_t.max)
    {
        auto ptr = g_threadBlock.memory + freeOffset;
        auto header = cast(MemoryHeader*)(ptr - MemoryHeader.sizeof);

        header.size = cast(uint) size;

        recordAlloc(header.offset + g_threadBlock.globalOffset, size);

        return ptr;
    }

    auto reqSize = size;

    size += MemoryHeader.sizeof;

    assert(g_threadBlock.blockOffset + size < g_threadBlock.blockSize, "Exceeded max block size for this thread");

    auto alignedOffset = alignTo(g_threadBlock.blockOffset, alignment);

    auto memory = g_threadBlock.memory + alignedOffset;

    import core.stdc.string : memcpy;

    MemoryHeader header = {
        offset: cast(uint) alignedOffset + cast(uint) MemoryHeader.sizeof,
        size: cast(uint) reqSize,
    };
    memcpy(memory, &header, MemoryHeader.sizeof);

    g_threadBlock.blockOffset = alignedOffset + size;

    g_statisticsTable[g_threadBlock.index].inUse += reqSize;

    recordAlloc(header.offset + g_threadBlock.globalOffset, reqSize);

    return memory + MemoryHeader.sizeof;
}

void ffree(byte* ptr)
{
    import forge.mem.utils;

    auto header = cast(MemoryHeader*)(ptr - MemoryHeader.sizeof);

    auto size = header.size;

    size_t index;

    if (size < MinSC)
    {
        index = 0;
    }
    else if (size > MaxSC)
    {
        index = MaxSizeClasses - 1;
    }
    else
    {
        // + 1 to move past tiny sc
        index = toSizeClass(size) + 1;
    }

    auto ref sc = g_threadBlock.sizeClasses[index];

    import std.format;

    debug assert(sc.offset < sc.maxSize, format(
            "free list for size class of index %s ran out of memory", toSizeClass(size)));

    *(sc.list + sc.offset) = header.offset;

    sc.offset++;

    auto ref stats = g_statisticsTable[g_threadBlock.index];

    stats.inUse -= size;
    stats.free += size;

    removeAlloc(header.offset + g_threadBlock.globalOffset);
}

private
{
    T toNextPower2(T)(T n)
    {
        n--;

        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        static if (T.sizeof == 8)
        {
            n |= n >> 32;
        }

        return n + 1;
    }

    size_t toSizeClass(size_t size)
    {
        import std.math;
        import std.algorithm;

        if (size < 32)
        {
            size = 32;
        }

        size = toNextPower2(size);

        const index = cast(size_t) log2(size) - 5;

        return clamp(index, 0, MaxSizeClasses - 1);
    }

    size_t findFree(size_t size)
    {
        auto ref sc = g_threadBlock.sizeClasses[toSizeClass(size)];

        if (sc.offset > 0)
        {
            auto freeOffset = *sc.list;
            sc.offset--;
            return freeOffset;
        }

        return size_t.max;
    }

    void recordAlloc(size_t id, size_t size)
    {
        debug (RecordAllocations)
        {
            import core.runtime;

            AllocationFootprint footprint;

            footprint.trace = defaultTraceHandler(null);
            footprint.size = size;

            auto ref stats = g_statisticsTable[g_threadBlock.index];

            synchronized stats.footprint[id] = footprint;
        }
    }

    void removeAlloc(size_t id)
    {
        debug (RecordAllocations)
        {
            synchronized
            {
                auto ref stats = g_statisticsTable[g_threadBlock.index];

                stats.footprint.remove(id);
            }
        }
    }
}
