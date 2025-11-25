module forge.container.string;

import core.stdc.string : memcpy, memset;
import std.typecons;
import ascii = std.ascii;

import forge.mem.allocators;
import forge.container.mixins;

// a dynamically sized string struct that can take an allocator
struct BaseString(T, Allocator = DefaultAllocator!T)
{
	union
	{
		T* data;
		T* ptr;
	}

	Allocator allocator;

	private
	{
		uint m_length;
		uint m_capacity;
	}

	mixin Container;

	this(S)(const auto ref S str)
	{
		append(str);
	}

	auto clone()
	{
	    typeof(this) str;

		str.m_length = m_length;
        str.m_capacity = m_capacity;
        str.allocator = allocator;

        str.data = allocator.alloc(m_capacity);

        memcpy(str.data, data, m_capacity);

        return str;
	}

	~this()
	{
	    if (data)
		{
            allocator.dealloc(data);
		}
	}

	void append(S)(const S str)
	{
		if (m_length + str.length >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? str.length * 2 : m_capacity * 2;

			reserve(cast(uint) newCapacity);
		}

		memcpy(data + m_length, str.ptr, str.length);

		m_length += cast(uint) str.length;
	}

	void append(const char c)
	{
		if (m_length + 1 >= m_capacity)
		{
			auto newCapacity = m_capacity == 0 ? 4 : m_capacity * 2;
			reserve(cast(uint) newCapacity);
		}

		data[m_length++] = c;
	}

	T pop()
	{
		if (m_length <= 0)
		{
			return T.init;
		}

		return data[--m_length];
	}

	void clear()
	{
        m_length = 0;
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

	int indexOf(S)(const S s) const pure
	{
		for (auto i = 0; i < m_length - s.length; i++)
		{
			if (data[i..i+s.length] == s)
			{
				return i;
			}
		}

		return -1;
	}

	bool contains(S)(const S s) const pure
	{
		return indexOf(s) != -1;
	}

	bool startsWith(S)(const S s) const pure
	{
		if (s.length > m_length)
		{
			return false;
		}

		return data[0..s.length] == s;
	}

	bool endsWith(S)(const S s) const pure
	{
		if (s.length > m_length)
		{
			return false;
		}

		return data[m_length-s.length..m_length] == s;
	}

	auto substr(uint start, uint count)
	{
		return BaseString!(T, Allocator)(data[start..start+count]);
	}

	@property
	length() const pure
	{
		return m_length;
	}

	@property
	capacity() const pure
	{
		return m_capacity;
	}

	@property
	void capacity(uint capacity)
	{
		reserve(capacity);
	}

	void reserve(uint newCapacity)
	{
		auto temp = allocator.alloc(newCapacity);

		memcpy(temp, data, m_length);

		m_capacity = newCapacity;

		if (data != null)
		{
			allocator.dealloc(data);
		}

		data = temp;
	}

	void resize(uint newSize)
	{
		if (newSize > m_length)
		{
			memset(data + m_length, 0, newSize - m_length);
		}

		m_length = newSize;
	}

	inout(T)[] slice() inout pure
	{
		return data[0 .. m_length];
	}

	string toString() const pure
	{
		return cast(string) slice();
	}

	bool opEquals(S)(const S s) const pure
	{
		return slice() == s;
	}

	size_t toHash() const pure nothrow
	{
	    return toString().hashOf;
	}

	void opOpAssign(string op : "+", S)(const S s)
	{
		append(s);
	}

	ref inout(T) opIndex(uint i) inout pure
	{
		return data[i];
	}

	inout(T)[] opSlice(uint i, uint j) inout pure
	{
		return data[i .. j];
	}

	int opApply(scope int delegate(ref T) dg)
	{
		foreach (ref val; data[0 .. m_length])
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
		foreach (ref val; data[0 .. m_length])
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
		foreach (i, ref val; data[0 .. m_length])
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
		foreach (i, ref val; data[0 .. m_length])
		{
			auto result = dg(i, val);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}

	void write(T)(T input)
	{
	    append(input);
	}

	void read(byte[] bytes)
	{
	    import std.algorithm;

	    auto len = min(m_length, bytes.length);

		memcpy(bytes.ptr, ptr, len);
	}
}

// a dynamically sized string struct
alias String = BaseString!(char, DefaultAllocator!char);
