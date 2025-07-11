module reflection_test;

import std.stdio;
import std.array;
import std.string;

struct Export
{
}

void tupleofTest(T)(ref T obj)
{
    foreach (i, ref field; obj.tupleof)
    {
        enum fieldName = T.tupleof[i].stringof;
        // enum fieldName = __traits(identifier, typeof(data).tupleof[i]);
        // enum attribName = __traits(identifier, __traits(getMember, Data, __traits(identifier, Data.tupleof[i])));

        alias Member = __traits(getMember, T, fieldName);

        auto visibility = __traits(getVisibility, Member);

        if (visibility != "public")
        {
            continue;
        }

        foreach (attr; __traits(getAttributes, Member))
        {
            static if (is(attr == Export))
            {
                writeln(typeof(field).stringof, " ", fieldName, ": ", field);

                break;
            }
        }
    }
}

void writeIndent(int depth)
{
    write("\t".replicate(depth));
    // for (auto i = 0; i < depth; i++)
    // {
    //     write("\t");
    // }
}

void reflect(T)(ref T obj, int depth = 0, string fieldName = "")
{

    writeIndent(depth);

    alias ObjType = typeof(obj);

    // writeln(i"$(T.stringof) $(fieldName):");

    enum members = __traits(allMembers, T);
    foreach (memberName; members)
    {
        static if (
            memberName != "init" &&
            memberName != "this" &&
            memberName != "~this" &&
            memberName != "mangleof" &&
            memberName != "stringof" &&
            memberName != "tupleof" &&
            memberName != "toString" &&
            memberName != "toHash" &&
            memberName != "opCmp" &&
            memberName != "opEquals" &&
            memberName != "Monitor" &&
            memberName != "factory" &&
            !memberName.startsWith("__")
            )
        {
            // Check if it's a real field you can access
            static if (__traits(compiles, __traits(getMember, obj, memberName)))
            {
                alias FieldType = typeof(__traits(getMember, obj, memberName));
                auto ref value = __traits(getMember, obj, memberName);

                writeIndent(depth + 1);

                static if (is(typeof(value) == struct))
                {
                    reflect(value, ++depth, memberName);
                }
                else
                {
                    // writeln(i"$(FieldType.stringof) $(memberName) = $(value)");
                }
            }
        }
    }
}
