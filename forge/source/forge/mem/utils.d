module forge.mem.utils;

@nogc:

static T alignTo(T)(T value, T alignment) pure
{
    auto mask = alignment - 1;
    return (value + mask) & ~mask;
}

static size_t getTypeAlignment(T)() pure
{
    size_t alignment;

    static if (is(T == class))
    {
        alignment = __traits(classInstanceAlignment, T);
    }
    else
    {
        alignment = T.alignof;
    }

    return alignment;
}

static size_t getTypeSize(T)() pure
{
    size_t size;

    static if (is(T == class))
    {
        size = __traits(classInstanceSize, T);
        // size = alignTo(size, __traits(classInstanceAlignment, T));
    }
    else
    {
        size = T.sizeof;
    }

    return size;
}

// if T is a class than it will be itself otherwise it will be a pointer
template RefOrPtr(T)
{
	static if (is(T == class))
	{
		alias RefOrPtr = T;
	}
	else
	{
		alias RefOrPtr = T*;
	}
}
