module forge.container.mixins;

mixin template Container()
{
    @disable this(this);
    @disable void opAssign(ref typeof(this) rhs);
}
