import forge.container.string;
import forge.container.string_view;
import forge.container.list;
import std.stdio;
import core.stdc.string;
import std.conv;
import std.conv : to;
import forge.fmt;
import std.typecons;
import forge.mem.allocators;
import forge.mem.box;

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

    //     println(str2);
    // }

    // println("{}", str.length);

    // str.append("hello");

    // auto f = 32.0f;

    // my_write_line(str, 5, f);

    // import std.digest.murmurhash;
    //
    //
    //
    // String str = "haoo";
    // StringView sv = str;

    // println("{}", sv);
    //
    // auto data = Data2(100, "john");

    // auto list = List!Data2();

    // list.append(data);
    // list.append(Data2(200, "henry"));
    // list.append(Data2(200, "henry"));
    // list.append(Data2(200, "henry"));
    // list.append(Data2(200, "henry"));

    // auto d = list.swapPop(0);

    // println("{}", d);

    // // list.append(data);
    // foreach (ref item; list)
    // {
    // 	println("{}", item);
    // }

    // {

    auto data = Box!Data(100, "john");

    println("{}", data);

	   //  auto data = new Data(100, "john");

	   //  auto list = List!Data();

	   //  list.append(data);
	   //  list.append(new Data(200, "henry"));
	   //  list.append(new Data(200, "henry"));
	   //  list.append(new Data(200, "henry"));
	   //  list.append(new Data(200, "henry"));

	   //  foreach (ref item; list)
	   //  {
	   //      println("{}", item);
	   //  }

    // }

    // import forge.mem.allocators.fmalloc;
    // auto stats = getStatistics();

    // println("{}", stats);
    // list.clear();
    // println("{}", list.slice);
    // println("{}", typeof(*String.init.ptr).stringof);

    // struct Yes
    // {
    // 	alias Ok = int;
    // }

    // println("{}", ElementType!(string));
    // auto file = File("test.txt", "w+");

    // formatToWriter(stdout, "x = {}", 10);
    // scope auto data = new Data(100, "john");

    // formatToWriter(stdout, "{}", data);
}
