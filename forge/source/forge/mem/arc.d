module forge.mem.arc;

import forge.mem.utils;
import forge.mem.allocators;
import core.lifetime;

import core.atomic;

struct Arc(T)
{
	private struct Header
	{
		align(getTypeAlignment!T)
		byte[getTypeSize!T()] memory;

		shared int refCount;

		alias memory this;
	}

	@property
	ref T get()
	{
		return *(cast(T*)m_header.memory);
	}

	@property
	T* ptr()
	{
		return cast(T*)m_header.memory;
	}

	private Header *m_header;

	alias get this;

	this(Args...)(const auto ref Args args)
	{
		m_header = newObj!Header();

		auto obj = emplace!T(m_header.memory, forward!args);

		static if (is (T == class))
		{
			*(cast(T*)m_header.memory) = obj;
		}

		atomicOp!"+="(m_header.refCount, 1);
	}

	this(typeof(this) other)
	{
		copy(other);
	}

	void opAssign(ref typeof(this) rhs)
	{
		copy(rhs);
	}

	void copy(typeof(this) other)
	{
		if (other.m_header)
		{
			m_header = other.m_header;
			atomicOp!"+="(m_header.refCount, 1);
		}
	}

	~this()
	{
		if (m_header)
		{
			auto result = atomicOp!"-="(m_header.refCount, 1);

			if (result <= 0)
			{
				destroy!false(get());
				delObj(m_header);
				m_header = null;
			}
		}
	}
}
