module forge.container.contigious;

import forge.mem.allocators;

import forge.container;

import std.traits;

enum StringLike(T) = __traits(compiles, { auto s = T.init; auto p = s.ptr; }) && (isSomeChar!(typeof(*T.init.ptr)) || is(typeof(*T.init.ptr) == char));

// mixin definitions for a contigious container

mixin template ContigiousCore(T, bool View = false)
{
	mixin Container!View;

	T* ptr;

	private uint m_length;

	@property
	length() const pure
	{
		return m_length;
	}

	@property
	inout(T)[] slice() inout pure
	{
		return ptr[0 .. m_length];
	}

	bool opEquals(S)(const auto ref S s) const pure
	{
		return slice() == s;
	}

	ref inout(T) opIndex(uint i) inout pure
	{
		return ptr[i];
	}

	inout(T)[] opSlice(uint i, uint j) inout pure
	{
		return ptr[i .. j];
	}

	int opApply(scope int delegate(ref T) dg)
	{
		foreach (ref val; ptr[0 .. m_length])
		{
			auto result = dg(val);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}

	// i cannot get inout to work. The compiler is literally lying to me and the feds are out to get me.
	// TODO: come back to this when i know d better or when ive taken my meds
	int opApply(scope int delegate(const ref T) dg) const
	{
		foreach (ref val; ptr[0 .. m_length])
		{
			auto result = dg(val);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}

	int opApply(scope int delegate(size_t, ref T) dg)
	{
		foreach (i, ref val; ptr[0 .. m_length])
		{
			auto result = dg(i, val);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}

	int opApply(scope int delegate(size_t, const ref T) dg) const
	{
		foreach (i, ref val; ptr[0 .. m_length])
		{
			auto result = dg(i, val);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}
}

mixin template ContigiousWrite(T, Allocator = DefaultAllocator!T)
{
	Allocator allocator;

	private uint m_capacity;

	@property
	capacity() const pure
	{
		return m_capacity;
	}

	this(S)(const auto ref S str)
	{
		append(str);
	}

	~this()
	{
	    if (ptr)
		{
            allocator.dealloc(ptr);
		}
	}

	void checkCapacity(uint increase)
	{
		if (increase >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? increase * 2 : m_capacity * 2;

			reserve(cast(uint) newCapacity);
		}
	}

	T pop()
	{
		if (m_length <= 0)
		{
			return T.init;
		}

		import core.lifetime;

		return ptr[--m_length].move();
	}

	auto clone()
	{
		import core.stdc.string;

	    typeof(this) newSelf;

		newSelf.m_length = m_length;
		newSelf.m_capacity = m_capacity;
		newSelf.allocator = allocator;

		newSelf.ptr = allocator.alloc(m_capacity);

		memcpy(newSelf.ptr, ptr, m_capacity);

		return newSelf;
	}

	void clear()
	{
		static if (!__traits(isPOD, T) || __traits(hasMember, T, "__dtor") || is(T == class))
		{
			import core.lifetime;

			foreach (ref item; this)
			{
				destroy!false(item);
			}
		}

        m_length = 0;
	}

	void reserve(uint newCapacity)
	{
		import core.stdc.string;

		auto temp = allocator.alloc(newCapacity);

		static if (is(T == class) || !__traits(isPOD, T))
		{
			import core.lifetime;

			foreach (i, ref item; this)
			{
				emplace(&temp[i], move(item));
			}
		}
		else
		{
			memcpy(temp, ptr, m_length);
		}

		m_capacity = newCapacity;

		if (ptr != null)
		{
			allocator.dealloc(ptr);
		}

		ptr = temp;
	}

	void resize(uint newSize)
	{
		import core.stdc.string;
		if (newSize > m_length)
		{
			memset(ptr + m_length, 0, newSize - m_length);
		}

		m_length = newSize;
	}

	void write(T)(T input)
	{
	    append(input);
	}
}

mixin template ContigiousRead(T)
{
	int indexOf(B)(const auto ref B target) const
	if (__traits(compiles, this[0] == target))
	{
		foreach (i, ref item; this)
		{
			if (item == target)
			{
				return cast(int)i;
			}
		}

		return -1;
	}

	bool contains(S)(const auto ref S s) const
	{
		return indexOf(s) != -1;
	}
}

// only string read is defined because it is only one needed to share between a String and a StringView
mixin template StringRead(T)
{
	int indexOf(S)(const auto ref S target) const
	if (StringLike!S)
	{
		import std.math;

		auto diff = abs(m_length - target.length);

		for (auto i = 0; i < diff; i++)
		{
			if (ptr[i..i+target.length] == target)
			{
				return i;
			}
		}

		return -1;
	}

	bool startsWith(S)(const S s) const pure
	{
		if (s.length > m_length)
		{
			return false;
		}

		return ptr[0..s.length] == s;
	}

	bool endsWith(S)(const S s) const pure
	{
		if (s.length > m_length)
		{
			return false;
		}

		return ptr[m_length-s.length..m_length] == s;
	}

	string toString() const pure
	{
		return cast(string) slice();
	}

	size_t toHash() const pure nothrow
	{
	    return toString().hashOf;
	}

	void read(byte[] bytes)
	{
	    import std.algorithm;
		import core.stdc.string : memcpy;

	    auto len = min(m_length, bytes.length);

		memcpy(bytes.ptr, ptr, len);
	}
}
