import forge.container.string;
import std.stdio;
import core.stdc.string;
import std.conv;

class Data
{
    int score;
    String name;
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

    String str = "HELLO world";

    str.toLower();

    writeln(str);
}
