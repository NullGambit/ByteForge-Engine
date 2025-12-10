module forge.container.list;

import forge.mem.allocators;
import forge.container.contigious;
import forge.container.core;

import forge.mem.allocators;

struct List(T, alias Allocator = DefaultAllocator)
{
    mixin Allocator allocator;

	mixin ContigiousCore!T;
	mixin ContigiousRead!T;
	mixin ContigiousWrite!(T);

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

unittest
{
    List!int list;

    list.append(1);
    list.append(2);
    list.append(3);
    list.append(4);

    assert(list.length == 4);

    assert(list.back == 4);
    assert(list.back == list[list.length-1]);

    import forge.fmt;

    assert(list.front == 1);
    assert(list.front == list[0]);

    foreach (i, item; list)
    {
        assert(i + 1 == item);
    }

    list.resize(8);

    assert(list.length == 8);

    list[6] = 10;

    assert(list[5] == int.init);
    assert(list[6] == 10);
    assert(list[7] == int.init);

    assert(list.contains(3));
}
