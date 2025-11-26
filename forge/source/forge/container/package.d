module forge.container;

mixin template Container()
{
	alias ElementType = T;

    @disable this(this);
    @disable void opAssign(ref typeof(this) rhs);
}
