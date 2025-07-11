module forge.mem.defs;

size_t kbs(size_t n = 1) pure
{
    return n * 1024UL;
}

size_t mbs(size_t n = 1) pure
{
    return n * 1024UL * 1024UL;
}

size_t gbs(size_t n = 1) pure
{
    return n * 1024UL * 1024UL * 1024UL;
}
