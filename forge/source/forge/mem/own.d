module forge.mem.own;

import forge.mem.utils;
import core.lifetime;

struct Own(T)
if (is (T == class))
{
	alias aligned = _alignUp!(getTypeAlignment!T);

	byte[aligned(__traits(classInstanceSize, T) + size_t.sizeof) + getTypeAlignment!T] memory;

	alias get this;

	@disable this(this);
	@disable void opAssign(ref typeof(this) rhs);

	static size_t _alignUp(size_t alignment)(size_t n)
	if (alignment > 0 && !((alignment - 1) & alignment))
	{
	    enum badEnd = alignment - 1; // 0b11, 0b111, ...
	    return (n + badEnd) & ~badEnd;
	}

	this(Args...)(auto ref Args args)
	{
        auto alignedStore = cast(byte*) aligned(cast(size_t) memory.ptr);
        immutable size_t d = alignedStore - memory.ptr;
        *cast(size_t*) &memory[$ - size_t.sizeof] = d;

        emplace!(T)(memory[d .. $ - size_t.sizeof], forward!args);
	}

	~this()
	{
		destroy!false(get);
	}

	@property
	inout(T) get() inout
	{
		auto alignedStore = cast(byte*) aligned(cast(size_t) memory.ptr);
        // As `Scoped` can be unaligned moved in memory class instance should be moved accordingly.
        immutable size_t d = alignedStore - memory.ptr;
        size_t* currD = cast(size_t*) &memory[$ - size_t.sizeof];
        if (d != *currD)
        {
            import core.stdc.string : memmove;
            memmove(alignedStore, memory.ptr + *currD, __traits(classInstanceSize, T));
            *currD = d;
        }
        return cast(inout(T)) alignedStore;
	}
}
