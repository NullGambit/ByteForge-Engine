module forge.fmt;

import core.sys.linux.unistd;
import forge.container.string;
import std.conv : to;
import core.lifetime;

struct HideInFormatting;

template Writer(T)
{
    // enum Writer = __traits(compiles,
    // {
    //     T t;
    //     byte[] b;
    //     t.write(b);
    // });
    //
    enum Writer = __traits(hasMember, T, "write");
}

void writer_reserve(W)(auto ref W w, uint amount)
{
    static if (__traits(compiles, w.reserve(amount)))
    {
        w.reserve(amount);
    }
}

void formatRecord(W, T)(auto ref W w, const auto ref T t)
{
	// TODO make it so it uses alias this instead of the actual type if it does have alias this
	import std.traits : hasUDA;

    w.write(T.stringof);
    w.write(" {  ");

    foreach (i, ref field; t.tupleof)
    {
        enum fieldName = T.tupleof[i].stringof;

        alias Member = __traits(getMember, T, fieldName);

        static if (hasUDA!(Member, HideInFormatting))
        {
            continue;
        }

        if (i > 0 && i < t.tupleof.length-1)
        {
            w.write(", ");
        }

        w.write(fieldName);
        w.write(": ");

        auto converted = to!string(field);

        w.write(converted);
        w.write(" ");
    }

    w.write(" }");
}

private void stringifyWrite(W, T)(ref W w, const ref T value)
{
    static if (__traits(compiles, value.init.toString(w)))
    {
        value.toString(w);
    }
    else static if (!__traits(compiles, value.init.toString()) && (is(typeof(value) == struct) || is(typeof(value) == class)))
    {
        formatRecord(w, value);
    }
    else
    {
        auto converted = to!string(forward!value);

        w.write(converted);
    }
}

void formatToWriter(W, F, Args...)(auto ref W w, const auto ref F fmt, const auto ref Args args)
if (Writer!W)
{
    import core.stdc.stdio;

    if (args.length == 0)
    {
        stringifyWrite(w, fmt);
        return;
    }

    auto start = 0;
    auto offset = 0;

    foreach (ref arg; args)
    {
        char c = fmt[offset++];

        while (c != '{')
        {
            c = fmt[offset++];
        }

        w.write(fmt[start..offset-1]);

        while (c != '}')
        {
            c = fmt[offset++];
        }

        start = offset;

        stringifyWrite(w, arg);
    }
}

String format(S, Args...)(const auto ref S fmt, const auto ref Args args)
{
    String s;

    formatToWriter(s, fmt, args);

    return s;
}

void println(S, Args...)(const auto ref S fmt, const auto ref Args args)
{
    import std.stdio;

    formatToWriter(stdout, fmt, forward!args);

    stdout.write('\n');
}

void print(S, Args...)(const auto ref S fmt, const auto ref Args args)
{
    import std.stdio;

    formatToWriter(stdout, fmt, forward!args);
}
