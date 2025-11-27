module forge.container;

mixin template Container(bool View = false)
{
	alias ElementType = T;

	static if (!View)
	{
	    @disable this(this);
	    @disable void opAssign(ref typeof(this) rhs);
	}
}
