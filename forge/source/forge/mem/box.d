module forge.mem.box;

import forge.mem.allocators;
import forge.mem.utils;

struct Box(T, alias allocFn = newObj, alias freeFn = delObj)
{
	RefOrPtr!T ptr;

	alias get this;

	@disable this(this);
	@disable void opAssign(ref typeof(this) rhs);

	this(Args...)(auto ref Args args)
	{
	    ptr = allocFn!T(args);
	}

	~this()
	{
		if (ptr !is null)
		{
			freeFn(ptr);
		}
	}

	@property
	ref T get()
	{
		static if (is (T == class))
		{
			return ptr;
		}
		else
		{
			return *ptr;
		}
	}
}
