module forge.container.list;

import forge.mem.allocators;
import forge.container.contigious;
import forge.container;

struct List(T, Allocator = DefaultAllocator!T)
{
	mixin ContigiousCore!T;
	mixin ContigiousRead!T;
	mixin ContigiousWrite!(T, Allocator);

	this(Args...)(auto ref Args args)
	{
		reserve(args.length);

		foreach (ref arg; args)
		{
			append(arg);
		}
	}

	void append(V)(auto ref V value)
	{
		import core.lifetime;
		import std.traits;

		checkCapacity(m_length + 1);

		static if (!isCopyable!V)
		{
			emplace(&ptr[m_length++], move(value));
		}
		else
		{
			ptr[m_length++] = value;
		}
	}
}
