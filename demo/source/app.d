import forge.container.string;
import forge.container.string_view;
import forge.container.list;
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
    import forge.container.map;

    Map!(String, int, SwissTableProbbing) map;

    map.put(String("zero"), 0);
    map.put(String("one"), 1);
    map.put(String("two"), 2);
    map.put(String("three"), 3);
    map.put(String("four"), 4);
    map.put("five", 5);
    map.put(String("six"), 6);
    map.put(String("seven"), 7);
    map.put(String("eight"), 8);

    // println("{}", map);
    //
    // map.remove(String("five"));
    // map["five"] = 10;
    //
    // map["a"] = 100;
    auto five = map.get("five");

    if (five)
    {
        println("{}", *five);
    }

    void printMap(T)(const ref T m)
    {
        foreach (ref key, ref value; m)
        {
            println("{}: {}", key, value);
        }
    }
    printMap(map);
}
