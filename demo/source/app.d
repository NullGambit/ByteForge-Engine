import forge.container.string;
import std.stdio;
import core.stdc.string;
import std.conv;
import std.conv : to;

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
}

struct Fields
{
    Data data;
}

import std.typecons;

String makeStr(string s)
{
    return String(s);
}

void main()
{
    //import forge.mem.allocators;
    //
    //auto ptr1 = newObj!Data();
    //auto ptr2 = newObj!Data();
    //auto ptr3 = newObj!Data();
    //
    //delObj(ptr2);
    //delObj(ptr1);
    //delObj(ptr3);
    //
    //auto data = newObj!Data();
    //
    //data.score += 10;
    //data.name.append("john");
    //
    //writeln(data.score, data.name);
    //
    //delObj(data);
    //String str;

    //const LEN = 40;
    //
    //foreach (i; 0..LEN)
    //{
    //    if (i == LEN / 2)
    //    {
    //        str.append("hello\n");
    //    }
    //    else
    //    {
    //        str.append(to!string(i));
    //        str.append('\n');
    //    }
    //}
    //
    // import core.lifetime;

    // String str = "hello world";

    // {
    //     auto str2 = str.move();

    //     my_write_line(str2);
    // }

    // str.append("hello");

    // auto f = 32.0f;

    // my_write_line(str, 5, f);

    // import std.digest.murmurhash;
    //
    //
    //

    import forge.fmt;

    // auto file = File("test.txt", "w+");

    // formatToWriter(stdout, "x = {}", 10);

    println("x = {}", 10);
}
