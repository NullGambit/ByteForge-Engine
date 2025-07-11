module mixins_test;

mixin template Adder(Args...)
{
    static foreach (T; Args)
    {
        import std.string;

        mixin(
            "T add" ~ T.stringof[0 .. 1].toUpper ~ T.stringof[1 .. $] ~ "(T a, T b)
		{
		    return a + b;
		}");
    }
}

void adderDriver()
{
    import std.stdio;

    mixin Adder!(int, float);

    auto result = addInt(5, 2);

    writeln(result);

    writeln(addFloat(5.5, 3.2));
}
