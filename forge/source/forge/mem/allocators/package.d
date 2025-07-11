module forge.mem.allocators;

import forge.mem.utils;
import forge.mem.allocators.fmalloc;

public:

byte* malloc(size_t size, size_t alignment = size_t.alignof) @nogc
{
    return fmalloc(size, alignment);
}

void free(byte* ptr) @nogc
{
    ffree(ptr);
}

private byte[] allocMemSpan(T)()
{
    enum size = getTypeSize!T();
    return malloc(size, getTypeAlignment!T())[0 .. size];
}

T newObj(T, Args...)(auto ref Args args) if (is(T == class))
{
    import std.conv : emplace;
    import core.lifetime : forward;

    auto memory = allocMemSpan!T();

    return emplace!T(memory, forward!args);
}

T* newObj(T, Args...)(auto ref Args args) if (!is(T == class))
{
    import std.conv : emplace;
    import core.lifetime : forward;

    auto memory = allocMemSpan!T();

    emplace!T(memory, forward!args);

    return cast(T*) memory;
}

void delObj(T)(T obj) if (is(T == class))
{
    destroy!false(obj);

    free(cast(byte*) obj);
}

void delObj(T)(T* ptr) if (!is(T == class))
{
    destroy!false(ptr);

    free(cast(byte*) ptr);
}

// the default allocator used by all containers that just uses the alloc or free functions
struct DefaultAllocator(T)
{
    T* alloc(size_t count)
    {
        auto size = getTypeSize!T();
        return cast(T*) malloc(size * count);
    }

    void dealloc(T* ptr)
    {
        return free(cast(byte*) ptr);
    }

    byte* allocBytes(size_t size)
    {
        return malloc(size);
    }

    void deallocBytes(byte* ptr)
    {
        return free(ptr);
    }
}
