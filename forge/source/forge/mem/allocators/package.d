module forge.mem.allocators;

import forge.mem.utils;
import forge.mem.allocators.fmalloc;
import mimalloc;

public:

byte* malloc(size_t size, size_t alignment = size_t.alignof) @nogc
{
    version (use_mimalloc)
    {
        return cast(byte*) mi_aligned_alloc(alignment, size);
    }
    else
    {
        return fmalloc(size, alignment);
    }
}

void free(byte* ptr, size_t alignment = size_t.alignof) @nogc
{
    version (use_mimalloc)
    {
        mi_free_aligned(ptr, alignment);
    }
    else
    {
        ffree(ptr);
    }
}

private byte[] allocMemSpan(T)()
{
    enum size = getTypeSize!T();
    return malloc(size, getTypeAlignment!T())[0 .. size];
}

RefOrPtr!T newObj(T, Args...)(auto ref Args args)
{
    import std.conv : emplace;
    import core.lifetime : forward;

    auto memory = allocMemSpan!T();

	static if (is (T == class))
	{
		return emplace!T(memory, forward!args);
	}
	else
	{
	    emplace!T(memory, forward!args);

	    return cast(T*) memory;
	}
}

void delObj(T)(T obj)
if (is (T == class))
{
    destroy!false(obj);

    free(cast(byte*) obj, getTypeAlignment!T());
}

void delObj(T)(T *obj)
if (!is (T == class))
{
    destroy!false(obj);

    free(cast(byte*) obj, getTypeAlignment!T());
}

// the default allocator used by all containers that just uses the malloc or free functions
mixin template DefaultAllocator()
{
    import forge.mem.utils;

    static T* alloc(T)(size_t count = 1, size_t alignment = getTypeAlignment!T())
    {
        auto size = getTypeSize!T();

        return cast(T*) malloc(size * count, alignment);
    }

    static void dealloc(T)(T* ptr, size_t alignment = getTypeAlignment!T())
    {
        import forge.mem.utils;

        return free(cast(byte*) ptr, alignment);
    }
}

// an allocator that allocates on the static and never frees. getTotal is applicable and should be used for efficiency
mixin template StaticALlocator(T, size_t N)
{
	T[N] memory;
	uint offset;

	// U is just a dummy so it works the same as other allocators
    T* alloc(U)(size_t count = 1, size_t alignment = getTypeAlignment!T())
    {
        auto size = (getTypeSize!T() + alignment) * count;

        auto ptr =  cast(T*) memory[offset..offset + size];

        offset += size;

        return ptr;
    }

    void dealloc(T* ptr, size_t alignment = getTypeAlignment!T())
    {}

    static size_t getTotal()
    {
    	return N;
    }
}
