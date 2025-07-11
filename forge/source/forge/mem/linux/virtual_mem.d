module forge.mem.linux.virtual_mem;

version (linux)  : import core.sys.linux.sys.mman;

import forge.mem.utils;

@nogc:

private size_t _SYS_PAGE_SIZE;

private const MAP_HUGE_2MB = 21 << MAP_HUGE_SHIFT;
private const MAP_HUGE_1GB = 30 << MAP_HUGE_SHIFT;

@property
sysPageSize()
{
    return _SYS_PAGE_SIZE;
}

static this()
{
    import core.sys.linux.unistd;

    _SYS_PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
}

byte* virtualAlloc(size_t size)
{
    static auto n = size_t.sizeof;

    size = alignTo(size + 20, _SYS_PAGE_SIZE);

    auto ptr = cast(byte*) mmap(null, size + n,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == cast(void*)-1)
    {
        return null;
    }

    import core.stdc.string : memcpy;

    memcpy(ptr, &size, n);

    return ptr + n;
}

void virtualFree(byte* ptr)
{
    if (!ptr)
    {
        return;
    }

    auto size = cast(size_t)(ptr - size_t.sizeof);

    munmap(ptr, size);
}
