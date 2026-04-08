module forge.container.string;

import core.stdc.string : memcpy, memset;
import std.typecons;
import ascii = std.ascii;

import forge.mem.allocators;
import forge.container.core;
import forge.container.contigious;

@nogc:

// a dynamically sized string struct that can take an allocator
struct BaseString(T, alias Allocator = DefaultAllocator, bool NullTerminate = false)
{
    mixin Allocator allocator;

	mixin ContigiousCore!T;
	mixin ContigiousRead!T;
	mixin ContigiousWrite!(T);
	mixin StringRead!T;

	void append(S)(const auto ref S value)
	if (StringLike!S)
	{
	    auto lengthModifier = cast(uint)value.length;

		static if (NullTerminate)
		{
		    lengthModifier++;
		}

		checkCapacity(m_length + lengthModifier);

		memcpy(ptr + m_length, value.ptr, value.length);

		m_length += cast(uint) value.length;

		static if (NullTerminate)
		{
		    ptr[m_length + 1] = '\0';
		}
	}

	void append(const char c)
	{
		if (m_length + 1 >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? 4 : m_capacity * 2;
			reserve(cast(uint) newCapacity);
		}

		ptr[m_length++] = c;

		static if (NullTerminate)
		{
		    ptr[m_length + 1] = '\0';
		}
	}

	void toUpper()
	{
		foreach (ref c; slice())
		{
			c = ascii.toUpper(c);
		}
	}

	void toLower()
	{
		foreach (ref c; slice())
		{
			c = ascii.toLower(c);
		}
	}

	auto substr(uint start, uint count)
	{
		return BaseString!(T, Allocator)(ptr[start..start+count]);
	}

	void opOpAssign(string op : "+", S)(const S s)
	if (StringLike!S)
	{
		append(s);
	}

	BaseString!(T, CsAllocator, true) toCstring(alias CsAllocator = DefaultAllocator)()
	{
	    static if (NullTerminate)
		{
		    return this;
		}

	    import std.algorithm.mutation : move;

	    BaseString!(T, CsAllocator, true) cstring = move(this);

		const n = cstring.length + 1;

		cstring.reserve(n);

		cstring.ptr[n] = '\0';

		return cstring;
	}
}

// a dynamically sized string struct
alias String = BaseString!(char, DefaultAllocator, false);
// a null terminated dynamically sized string struct that is compatible with c
alias Cstring = BaseString!(char, DefaultAllocator, true);

unittest
{
    Cstring str;

    str.append("hello");

    auto len = str.length+1;

    assert(str.ptr[len] == '\0');

    str.append(' ');

    assert(str.ptr[len] == '\0');
    assert(str.ptr[str.length+1] == '\0');

    str.append("World");

    assert(str.ptr[str.length+1] == '\0');

    String s1 = "hello";

    auto cs1 = s1.toCstring;

    assert(cs1.ptr[cs1.length+1] == '\0');
}
