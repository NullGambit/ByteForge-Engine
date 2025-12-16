module forge.mem.utils;

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

template clone(T)
{
	import std.traits;
	import core.lifetime;

	T clone(auto ref T src)
	{
		static if (__traits(hasMember, T, "clone"))
		{
			return src.clone();
		}
		else static if (is(T == class) || is(T == struct))
		{
			T cpy;

			foreach (i, ref field; src.tupleof)
			{
				emplace(&cpy.tupleof[i], clone(field));
			}

			return cpy;
		}
		else
		{
			return src;
		}
	}
}

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
