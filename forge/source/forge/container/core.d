module forge.container.core;

template Container(bool View = false)
{
	alias ElementType = T;

	static if (!View)
	{
	    @disable this(this);
	    @disable void opAssign(ref typeof(this) rhs);
	}
}
