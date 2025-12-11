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
    import bench.map_bench;

    mapBench();

    // import forge.digest;
    // import forge.container;

    // Map!(String, int, RobbinHoodProbing) map;

    // map["a"] = 1;
    // map["b"] = 2;
    // map["c"] = 3;
    // map["d"] = 4;

    // // map.put("d", 4);

    // map.put("hello", 1000);

    // map.put("remove me", -1);

    // map["c"] = 100;

    // map.remove("remove me");

    // println(map["a"] == 1);
    // println(map["b"] == 2);
    // println(map["c"] == 100);
    // println(*map.get("d"));
    // println(*map.get("d"));

    // println(xxhash64("d"));

    // import bench.hash_bench;

    // runHashBench();
}
