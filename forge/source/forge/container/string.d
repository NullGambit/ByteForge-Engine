module forge.container.string;

import core.stdc.string : memcpy, memset;
import std.typecons;
import ascii = std.ascii;

import forge.mem.allocators;
import forge.container;
import forge.container.contigious;

// a dynamically sized string struct that can take an allocator
struct BaseString(T, Allocator = DefaultAllocator!T)
{
	mixin ContigiousCore!T;
	mixin ContigiousRead!T;
	mixin ContigiousWrite!(T, Allocator);
	mixin StringRead!T;

	void append(S)(const auto ref S value)
	if (StringLike!S)
	{
		if (m_length + value.length >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? value.length * 2 : m_capacity * 2;

			reserve(cast(uint) newCapacity);
		}

		memcpy(ptr + m_length, value.ptr, value.length);

		m_length += cast(uint) value.length;
	}

	void append(const char c)
	{
		if (m_length + 1 >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? 4 : m_capacity * 2;
			reserve(cast(uint) newCapacity);
		}

		ptr[m_length++] = c;
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
}

// a dynamically sized string struct
alias String = BaseString!(char, DefaultAllocator!char);
