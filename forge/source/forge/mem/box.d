module forge.mem.box;

import forge.mem.allocators;

struct Box(T, Allocator = DefaultAllocator!T)
{
	static if (is(T == class))
	{
		T ptr;
	}
	else
	{
		T *ptr;
	}

	alias ptr this;

	@disable this(this);
	@disable void opAssign(ref typeof(this) rhs);

	this(Args...)(auto ref Args args)
	{
		ptr = newObj!T(args);
	}

	~this()
	{
		if (ptr !is null)
		{
			delObj(ptr);
		}
	}
}
