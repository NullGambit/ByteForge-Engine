module forge.result;

import std.sumtype;
import std.algorithm.mutation : move;

struct Result(Ok, Error)
{
    static if (is(Ok == void))
    {
        struct NoValue {}

        private SumType!(NoValue, Error) state;
    }
    else
    {
        private SumType!(Ok, Error) state;
    }

    this(T)(auto ref T value)
    {
        state = move(value);
    }

    bool isOk() pure
    {
        return !state.has!Error;
    }

    static if (!is(Ok == void))
    {
        alias get this;

        Ok* get()
        {
            if (isOk)
            {
                return &state.get!Ok;
            }

            return null;
        }
    }
}

template IsError(E)
{
    alias IsError = Result!(void, E);
}
