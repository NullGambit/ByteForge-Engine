module forge.math.vector;

import forge.fmt;

struct Vec(T, size_t S) if (is(T : real))
{
    alias V = Vec!(T, S);

    union
    {
        struct
        {
            static if (S >= 1)
            {
                T x;
            }
            static if (S >= 2)
            {
                T y;
            }
            static if (S >= 3)
            {
                T z;
            }
            static if (S >= 4)
            {
                T w;
            }
        }

        @HideInFormatting
        T[S] raw;
    }

    this(Args...)(Args args)
    {
        static assert(args.length <= S);

        static if (args.length == 1)
        {
            static foreach (i; 0..S)
            {
                raw[i] = args[0];
            }
        }
        else
        {
            static foreach (i; 0..args.length)
            {
                raw[i] = args[i];
            }
        }
    }

    V opBinary(string op)(V rhs)
    {
        V v;
        mixin("v.raw = raw[]" ~ op ~ "rhs.raw[];");
        return v;
    }

    V opBinary(string op)(T scalar)
    {
        V v;
        mixin("v.raw = raw[]" ~ op ~ "scalar;");
        return v;
    }

    void opOpAssign(string op)(V rhs)
    {
        mixin("raw[] " ~ op ~ "= rhs.raw[];");
    }

    void opOpAssign(string op)(T scalar)
    {
        mixin("raw[] " ~ op ~ "= scalar;");
    }

    V opUnary(string op)() if (op == "-")
    {
        V v;
        mixin("v.raw = " ~ op ~ "raw[];");
        return v;
    }

    void toString(W)(auto ref W w) const
    {
        w.write("Vec");
        w.write(S);
        w.write("(");

    	static foreach (i, field; this.tupleof[0..$-1])
	    {
			w.write(typeof(this).tupleof[i].stringof);
			w.write(": ");
			w.write(field);

			if (i < this.tupleof.length-2)
			{
			    w.write(", ");
			}
	    }

		w.write(")");
    }

    T squareLength() const pure
    {
        T result = 0.0f;

        static foreach (i; 0..S)
        {
            result += raw[i] * raw[i];
        }

        return result;
    }

    T length() const pure
    {
        import std.math;

        return sqrt(squareLength);
    }

    void normalize()
    {
        raw = raw[] / length;
    }
}

alias Vec2 = Vec!(float, 2);
alias Vec3 = Vec!(float, 3);
alias Vec4 = Vec!(float, 4);

auto dot(T)(const ref T a, const ref T b) pure
{
    auto result = 0.0f;

    static foreach (i; 0 .. a.raw.length)
    {
        result += a.raw[i] * b.raw[i];
    }

    return result;
}

auto normalize(T)(T vec) pure
{
    vec.normalize();
    return vec;
}
