module forge.fmt;

import core.sys.linux.unistd;
import forge.container.string;

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

void formatToWriter(W, S, Args...)(auto ref W w, const auto ref S fmt, const auto ref Args args)
if (Writer!W)
{
    import core.stdc.stdio;
    import std.conv : to;

    static string[args.length] str;

    auto possibleCap = cast(uint)fmt.length;

    static foreach (i, arg; args)
    {
        str[i] = to!string(args[i]);
        // +2 for '{}'
        possibleCap += str[i].length + 2;
    }

    writer_reserve(w, possibleCap);

    auto strIndex = 0;

    for (auto i = 0; i < fmt.length; i++)
    {
        char c = fmt[i];

        if (c == '{')
        {
            auto converted = str[strIndex++];

            w.write(converted);

            while (c != '}')
            {
                c = fmt[++i];
            }
        }
        else
        {
            w.write(c);
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
