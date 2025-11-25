module forge.fmt;

import core.sys.linux.unistd;
import forge.container.string;
import std.conv : to;

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
    w.write(T.stringof);
    w.write(" { ");

    foreach (i, ref field; t.tupleof)
    {
        enum fieldName = T.tupleof[i].stringof;

        w.write(fieldName);
        w.write(": ");

        auto converted = to!string(field);

        w.write(converted);

        if (i < t.tupleof.length-1)
        {
            w.write(", ");
        }
    }

    w.write(" }");
}

void formatToWriter(W, S, Args...)(auto ref W w, const auto ref S fmt, const auto ref Args args)
if (Writer!W)
{
    import core.stdc.stdio;

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

        start = offset+1;

        static if (__traits(compiles, arg.init.toString(w)))
        {
            arg.toString(w);
        }
        else static if (!__traits(compiles, arg.init.toString()) && (is(typeof(arg) == struct) || is(typeof(arg) == class)))
        {
            formatRecord(w, arg);
        }
        else
        {
            auto converted = to!string(arg);

            w.write(converted);
        }
    }

    w.write('\n');
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

    formatToWriter(stdout, fmt, args);
}
