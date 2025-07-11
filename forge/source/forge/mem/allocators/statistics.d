module forge.mem.allocators.statistics;

struct AllocatorStatistics
{
    size_t free;
    size_t inUse;
    size_t reserved;
    size_t blocks;
    // only available when debugVersion RecordAllocations is enabled
    AllocationFootprint[size_t] footprint;
}

struct AllocationFootprint
{
    Throwable.TraceInfo trace;
    size_t size;
}
