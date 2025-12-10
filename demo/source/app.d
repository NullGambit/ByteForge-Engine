import forge.container;
import core.stdc.string;
import std.conv;
import std.conv : to;
import forge.fmt;
import std.typecons;
import forge.mem.allocators;
import forge.mem.box;
import forge.mem.arc;
import forge.math.vector;

class Data
{
    int score;
    String name;

    this(S)(int score, auto ref S name)
    {
        this.score = score;
        this.name = String(name);
    }
}

struct Data2
{
    int score;
    String name;

    this(S)(int score, auto ref S name)
    {
        this.score = score;
        this.name = String(name);
    }
}

struct Fields
{
    Data data;
}

void main()
{
    import map_bench;

    mapBench();
}
