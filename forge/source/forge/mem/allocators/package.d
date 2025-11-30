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

template newObj(T)
{
	RefOrPtr!T newObj(Args...)(auto ref Args args)
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

// the default allocator used by all containers that just uses the alloc or free functions
struct DefaultAllocator(T)
{
    T* alloc(size_t count = 1)
    {
        auto size = getTypeSize!T();
        return cast(T*) malloc(size * count, getTypeAlignment!T());
    }

    void dealloc(T* ptr)
    {
        return free(cast(byte*) ptr, getTypeAlignment!T());
    }
}

struct StaticAllocator(T, size_t N)
{
	T[N] memory;
	uint offset;

    T* alloc(size_t count = 1)
    {
        auto size = getTypeSize!T() * count;

        auto ptr =  cast(T*) memory[offset..offset + size];

        offset += size;

        return ptr;
    }

    void dealloc(T* ptr)
    {
    }

    size_t getTotal()
    {
    	return N;
    }
}
